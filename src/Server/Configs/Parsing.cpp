#include "Server.hpp"

#include <cstddef>
#include <fstream>

static const std::vector<std::string> split_line(const std::string &str)
{
	std::vector<std::string> words;
	std::string word;
	bool in_quote = false;

	for (std::string::size_type i = 0; i < str.size(); ++i)
	{
		if (str[i] == '"' || str[i] == '\'')
			in_quote = !in_quote;

		if (!in_quote && str[i] == '#')
			break;

		if (in_quote)
		{
			word += str[i];
			continue;
		}

		if (std::isspace(str[i]) || str[i] == '=' || str[i] == ';' || str[i] == '{' || str[i] == '}')
		{
			if (!word.empty())
			{
				words.push_back(word);
				word.clear();
			}
			if (!std::isspace(str[i]))
				words.push_back(std::string(1, str[i]));
		}
		else
			word += str[i];
	}
	if (!word.empty())
		words.push_back(word);
	return words;
}

void Server::read_config()
{
	std::ifstream file(_config_file.c_str());
	if (!file.is_open())
		throw std::runtime_error("Failed to open file : " + std::string(strerror(errno)));

	size_t line_number = 0;
	std::string line;
	while (std::getline(file, line))
	{
		++line_number;
		std::vector<std::string> words = split_line(line);
		if (words.empty())
			_content_file[line_number] = std::vector<std::string>();
		else
			for (std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it)
				_content_file[line_number].push_back(*it);
	}

	file.close();
}

const std::vector<std::string> Server::get_value(const std::string &token)
{
	std::vector<std::string> result;

	if (next_word() != "=")
		throw std::runtime_error("Syntax error: Expected '=' after " + token + " at line " + to_string(_current_line));

	std::string word = next_word();
	while (word != "" && word != ";")
	{
		if ((word[0] == '"' && word[word.size() - 1] == '"') || (word[0] == '\'' && word[word.size() - 1] == '\''))
			result.push_back(word.substr(1, word.size() - 2));
		else
		{
			if (word != ";")
				throw std::runtime_error("Syntax error: Expected ';' after value at line " + to_string(_current_line));
			else
				throw std::runtime_error("Syntax error: Missing \" or ' at line " + to_string(_current_line));
		}
		word = next_word();
	}

	if (result.empty())
		throw std::runtime_error("Syntax error: Expected value after " + token + " at line " +
								 to_string(_current_line));

	if (word != ";")
		throw std::runtime_error("Syntax error: Expected ';' after value at line " + to_string(_current_line));

	return result;
}

const route Server::parse_route()
{
	route result;

	result.path = "";
	result.accepted_methods.push_back("GET");
	result.accepted_methods.push_back("POST");
	result.accepted_methods.push_back("DELETE");
	result.root = "";
	result.redirect = "";
	result.directory_listing = false;
	result.default_file = "index.html";
	result.cgi = std::map<std::string, std::string>();
	result.cgi_upload_path = "/tmp/upload";
	result.cgi_upload_enable = true;

	std::string word = next_word();
	while (word != "" && word != "}")
	{
		route_value(result, word);
		word = next_word();
	}

	if (word != "}")
		throw std::runtime_error("Syntax error: Missing '}' at line " + to_string(_current_line));

	if (result.path.empty())
		throw std::runtime_error("Syntax error: Missing path at line " + to_string(_current_line));

	return result;
}

const server Server::parse_server()
{
	server result;

	result.host = "localhost";
	result.port = 8080;
	result.root = "www/";
	result.default_file = "index.html";
	result.server_names = std::vector<std::string>();
	result.default_server = false;
	result.max_body_size = 1000000;
	result.error_pages = std::map<std::string, std::string>();
	result.routes = std::vector<route>();

	std::string word = next_word();
	while (word != "" && word != "}")
	{
		if (word == "route")
		{
			const std::string next = next_word();
			if (next != "{")
				throw std::runtime_error("Syntax error: Expected '{' after route at line " + to_string(_current_line));
			result.routes.push_back(parse_route());
		}
		else
			server_value(result, word);
		word = next_word();
	}

	if (word != "}")
		throw std::runtime_error("Syntax error: Missing '}' at line " + to_string(_current_line));

	return result;
}

void Server::parsing_config()
{
	std::string word = next_word();

	while (true)
	{
		if (word == "server")
		{
			std::string next = next_word();
			if (next != "{")
				throw std::runtime_error("Syntax error: Expected '{' after server at line " + to_string(_current_line));
			_servers.push_back(parse_server());
		}
		else
			throw std::runtime_error("Syntax error: Invalid keyword " + word + " at line " + to_string(_current_line));
		word = next_word();
		if (word == "")
			break;
	}
}
