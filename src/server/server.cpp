#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

/* Functions */
#include <cstddef>
#include <iostream>		 // std::cerr, std::endl
#include <sys/epoll.h> // epoll_create1, epoll_ctl, epoll_wait
#include <sys/types.h>

Server::Server(const server &serv, const std::map<unsigned short, std::string> &error_codes)
		: _server(serv), _error_codes(error_codes), _status_code(200)
{
}

Server::~Server()
{
	close(_server_fd);
}

void Server::init()
{
	_server_fd = _bind_socket(_server.host, _server.port);
	_set_nonblocking(_server_fd);

	if (listen(_server_fd, 10) == -1)
	{
		std::cerr << "Error: listen: " << strerror(errno) << std::endl;
		close(_server_fd);
		exit(EXIT_FAILURE);
	}
}

void Server::accept_connection(int epoll_fd)
{
	int client_fd = accept(_server_fd, NULL, NULL);
	if (client_fd == -1)
	{
		perror("accept");
		return;
	}
	_set_nonblocking(client_fd);
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = client_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
	{
		perror("epoll_ctl: client_fd");
		close(client_fd);
	}
}

void Server::handle_client(int client_fd)
{
	char buffer[BUFFER_SIZE];

	ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
	while (bytes_read > 0)
	{
		_requests[client_fd].append(buffer, bytes_read);
		if (_requests[client_fd].find("\r\n\r\n") != std::string::npos)
			break;
		bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
	}

	// check if the request is complete
	Request request(*this, _requests[client_fd].c_str(), client_fd);
	if (!request.is_complet())
		return;

	Response response(*this, request);
	response.send_response(client_fd);

	close(client_fd);
	_requests.erase(client_fd);
}
