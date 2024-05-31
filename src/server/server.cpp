#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Utils.hpp"

/* Functions */
#include <cstddef>		 // size_t
#include <iostream>		 // std::cerr, std::endl
#include <sys/epoll.h> // epoll_create1, epoll_ctl, epoll_wait
#include <sys/types.h> // epoll_event, epoll_create1, epoll_ctl, epoll_wait

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

bool Server::has_error() const
{
	return (_status_code >= 400);
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

void Server::handle_client(const int &client_fd)
{
	char buffer[BUFFER_SIZE];

	ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
	while (bytes_read > 0)
	{
		_requests[client_fd].concatenate_request(std::string(buffer, bytes_read));
		bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
	}
	if (bytes_read == -1)
	{
		perror("recv");
		close(client_fd);
		return;
	}

	if (_requests[client_fd].is_complet() || has_error())
	{
		std::cout << _requests[client_fd];
		Response response(*this, _requests[client_fd]);
		std::cout << " " << to_string(_error_codes) << std::endl;
		response.send_response(client_fd);
		close(client_fd);
	}
}
