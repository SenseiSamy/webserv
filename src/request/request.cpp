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
	std::istringstream iss_request(request);
	std::string line;
	bool in_body = false;

	std::getline(iss_request, line); // First line is the request line

	while (std::getline(iss_request, line))
	{
		if (line == "\r")
		{
			in_body = true;
			continue;
		}

		if (!in_body)
		{
			size_t colon_pos = line.find(":");
			if (colon_pos != std::string::npos)
			{
				std::string header_name = line.substr(0, colon_pos);
				std::string header_value = line.substr(colon_pos + 2); // Skip ": "
				_headers[header_name] = header_value;
			}
		}
		else
		{
			if (_body.size() + line.size() + 1 > _server.get_server().max_body_size)
			{
				_body.clear();

				break;
			}
			_body += line + "\n";
		}
	}

	if (_headers.count("Host") > 0)
		_server.set_host(_headers["Host"]);

	// Remove the last newline character from _body
	if (!_body.empty() && _body[_body.size() - 1] == '\n')
		_body.erase(_body.size() - 1);
}