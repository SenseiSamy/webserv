#include "Request.hpp"

/* Functions */
#include <iostream> // std::cout, std::endl
#include <sstream>	// std::istringstream

Request::Request(Server &server, const char *request, const int &client_fd)
		: _server(server), _client_fd(client_fd), _request(request)
{
	parse_request(request);

	std::cout << _server.get_server().host << ":" << _server.get_server().port << " - - \"" << _server.get_methods()
						<< " " << _server.get_uri() << " " << _server.get_version() << "\"" << std::endl;
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

	// Parse headers
	while (std::getline(iss, line))
	{
		if (line.empty())
			break;
		std::istringstream header(line);
		std::string key, value;
		std::getline(header, key, ':');
		std::getline(header, value);
		_headers[key] = value;
	}

	// Parse body
	while (std::getline(iss, line))
	{
		if (_body.size() >= _server.get_server().max_body_size)
			break;
		_body += line;
	}
}
