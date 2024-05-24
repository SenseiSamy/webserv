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
	Request request(*this, NULL, client_fd);
	while (!request.is_complete())
	{
		char buffer[BUFFER_SIZE];
		ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
		if (bytes_read == -1)
		{
			perror("recv");
			close(client_fd);
			return;
		}
		else if (bytes_read == 0)
			break;
		request += Request(*this, buffer, client_fd);
	}

	Response response(*this, request);
	if (request.get_method() == "GET")
		_get(request, response);
	else if (request.get_method() == "POST")
		_post(request, response);
	else if (request.get_method() == "PUT")
		_put(request, response);
	else if (request.get_method() == "DELETE")
		_delete(request, response);
	else
		response.generate_error_page(405);
}
