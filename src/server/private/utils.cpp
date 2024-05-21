#include "Utils.hpp"
#include "Response.hpp"
#include "Server.hpp"

/* Functions */
#include <iostream> // std::cerr, std::endl

unsigned short Server::_request_line(const std::string &request)
{
	const std::string first_line = request.substr(0, request.find("\r\n"));
	const std::vector<std::string> tokens = split(first_line, ' ');

	if (tokens.size() != 3)
		return 400;

	_methods = tokens[0];
	_uri = tokens[1];
	_version = tokens[2];

	// check if the method is valid
	if (_methods_map.find(_methods) == _methods_map.end())
		return 405;

	// check if the HTTP version is valid
	if (_version != "HTTP/1.1")
		return 505;

	return 200;
}

int Server::accept_client() const
{
	sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(client_fd, (sockaddr *)&client_addr, &client_addr_len);
	if (client_fd == -1)
	{
		std::cerr << "Error: accept" << std::endl;
		return client_fd;
	}

	return client_fd;
}

void Server::send_response(const Response &response) const
{
	send(_client_fd, response.get_response(), response.get_response_size(), 0);
}
