#include "Server.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <stdexcept>
#include <sys/types.h>

void Server::route_value(route &route, const std::string &word)
{
	const std::vector<std::string> value = get_value(word);

	if (word != "cgi" && value.size() != 1)
		throw std::runtime_error("Syntax error: Expected one value after " + word + " at line " + to_string(_current_line));
	if (word == "cgi" && value.size() != 2)
		throw std::runtime_error("Syntax error: Expected two values after cgi at line " + to_string(_current_line));

	if (word == "path")
		route.path = value[0];
	else if (word == "http_methods")
	{
		std::vector<std::string> valid_methods;

		valid_methods.push_back("GET");
		valid_methods.push_back("POST");
		valid_methods.push_back("DELETE");

		if (std::find(valid_methods.begin(), valid_methods.end(), value[0]) != valid_methods.end())
		{
			bool found = false;
			for (std::vector<std::string>::iterator it = route.accepted_methods.begin(); it != route.accepted_methods.end();
					 ++it)
			{
				if (*it == value[0])
				{
					found = true;
					break;
				}
			}
			if (!found)
				route.accepted_methods.push_back(value[0]);
			else
				throw std::runtime_error("Syntax error: Duplicate http method " + value[0] + " at line " +
																 to_string(_current_line));
		}
		else
			throw std::runtime_error("Syntax error: Unsupported http method " + value[0] + " at line " +
															 to_string(_current_line));
	}
	else if (word == "root")
		route.root = value[0];
	else if (word == "redirect")
		route.redirect = value[0];
	else if (word == "directory_listing")
	{
		if (value[0] != "true" && value[0] != "false")
			throw std::runtime_error("Syntax error: Invalid value for directory_listing at line " + to_string(_current_line));
		route.directory_listing = value[0] == "true";
	}
	else if (word == "default_file")
		route.default_file = value[0];
	else if (word == "cgi")
		route.cgi[value[0]] = value[1];
	else if (word == "cgi_upload_path")
		route.cgi_upload_path = value[0];
	else if (word == "cgi_upload_enable")
	{
		if (value[0] != "true" && value[0] != "false")
			throw std::runtime_error("Syntax error: Invalid value for cgi_upload_enable at line " + to_string(_current_line));
		route.cgi_upload_enable = value[0] == "true";
	}
	else
		throw std::runtime_error("Syntax error: Invalid keyword " + word + " at line " + to_string(_current_line));
}

void Server::server_value(server &server, const std::string &word)
{
	const std::vector<std::string> value = get_value(word);

	if (word != "error_pages" && value.size() != 1)
		throw std::runtime_error("Syntax error: Expected one value after " + word + " at line " + to_string(_current_line));
	if (word == "error_pages" && value.size() != 2)
		throw std::runtime_error("Syntax error: Expected at least one value after server_names at line " +
														 to_string(_current_line));

	if (word == "host")
		server.host = value[0];
	else if (word == "port")
		server.port = to_size_t(value[0]);
	else if (word == "root")
		server.root = value[0];
	else if (word == "server_names")
		server.server_names = value;
	else if (word == "default_server")
	{
		if (value[0] == "true")
			server.default_server = true;
		else if (value[0] == "false")
			server.default_server = false;
		else
			throw std::runtime_error("Syntax error: Invalid value for default_server at line " + to_string(_current_line));
	}
	else if (word == "max_body_size")
		server.max_body_size = to_size_t(value[0]);
	else if (word == "error_pages")
	{
		const ssize_t code = to_size_t(value[0]);

		if (_error_codes.find(code) == _error_codes.end())
			throw std::runtime_error("Syntax error: Invalid error code " + value[0] + " at line " + to_string(_current_line));
		server.error_pages[value[0]] = value[1];
	}
	else
		throw std::runtime_error("Syntax error: Invalid keyword " + word + " at line " + to_string(_current_line));
}

void Server::syntax_brackets()
{
	size_t open_brackets = 0;
	size_t close_brackets = 0;
	std::string word = next_word();

	while (word != "")
	{
		if (word == "server" || word == "route")
		{
			word = next_word();
			if (word != "{")
				throw std::runtime_error("Syntax error: Expected '{' after " + word + "at line " + to_string(_current_line));
			++open_brackets;
		}
		else if (word == "}")
			++close_brackets;
	}

	if (open_brackets != close_brackets)
		throw std::runtime_error("Syntax error: Missing '}' at line " + to_string(_current_line));
}
