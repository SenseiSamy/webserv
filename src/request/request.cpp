#include "Request.hpp"

#include <iostream> // std::cout, std::endl
#include <sstream>	// std::istringstream

Request::Request(Server &server) : _server(server)
{
	char buffer[4096];
	std::string request;

	int bytes_received = recv(_server.get_client_fd(), buffer, sizeof(buffer) - 1, 0);
	if (bytes_received < 0)
	{
		std::cerr << "Error: recv" << std::endl;
		return;
	}
	buffer[bytes_received] = '\0';
	request = std::string(buffer);

	parse_request(request);

	std::cout << server.get_server().host << " - - \"" << _server.get_methods() << " " << _server.get_uri() << " "
						<< _server.get_version() << "\"" << std::endl;
}

Request::~Request()
{
	std::cout << "Request destructor" << std::endl;
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
	if (!_body.empty() && _body.back() == '\n')
		_body.pop_back();
}