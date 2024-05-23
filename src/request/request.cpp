#include "Request.hpp"

/* Functions */
#include <iostream> // std::cout, std::endl
#include <sstream>	// std::istringstream

Request::Request(Server &server, const char *request, const int &client_fd)
		: _server(server), _client_fd(client_fd), _request(request)
{
	parse_request(request);

	std::cout << _server.get_server().host << ":" << _server.get_server().port << " - - \"" << _method << " " << _uri
						<< " " << _version << "\" ";
}

Request::~Request()
{
}

void Request::parse_request(const std::string &request)
{
	std::istringstream iss(request);
	std::string line;

	// Parse request line
	std::getline(iss, line);
	std::istringstream first_line(line);
	first_line >> _method >> _uri >> _version;
	if (_method.empty() || _uri.empty() || _version.empty())
	{
		_server.set_status_code(400);
		return;
	}

	if (_method != "GET" && _method != "POST" && _method != "PUT" && _method != "DELETE")
	{
		_server.set_status_code(405);
		return;
	}

	if (_version != "HTTP/1.0" && _version != "HTTP/1.1")
	{
		_server.set_status_code(505);
		return;
	}

	// Parse headers
	while (std::getline(iss, line))
	{
		if (line.empty())
			break;
		std::istringstream header(line);
		std::string key, value;
		std::getline(header, key, ':');
		if (header.eof() || key.empty())
		{
			_server.set_status_code(400);
			return;
		}
		std::getline(header, value);
		if (value.empty())
		{
			_server.set_status_code(400);
			return;
		}

		_headers[key] = value;
	}

	// Parse body
	while (std::getline(iss, line))
	{
		if (_body.size() >= _server.get_server().max_body_size)
		{
			_server.set_status_code(413);
			return; // Set code to 413 and return
		}
		_body += line;
	}
}
