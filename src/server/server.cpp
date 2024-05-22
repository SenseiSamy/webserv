#include "Server.hpp"
#include "Utils.hpp"

/* Functions */
#include <cstring>		 // strerror
#include <iostream>		 // std::cerr, std::endl
#include <sys/epoll.h> // epoll_create1, epoll_ctl, epoll_wait
#include <unistd.h>		 // close

Server::Server(const server &serv) : _error_codes(init_error_codes()), _server(serv)
{
	_server_fd = _bind_socket(_server.host, _server.port);
	_set_nonblocking(_server_fd);

	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
	{
		std::cerr << "Error: epoll_create1: " << strerror(errno) << std::endl;
		close(_server_fd);
		exit(EXIT_FAILURE);
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = _server_fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_fd, &ev) == -1)
	{
		std::cerr << "Error: epoll_ctl: server_fd: " << strerror(errno) << std::endl;
		close(_server_fd);
		exit(EXIT_FAILURE);
	}
}

void Server::run()
{
	struct epoll_event events[MAX_EVENTS];
	while (true)
	{
		int event_count = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
		if (event_count == -1)
		{
			perror("epoll_wait");
			close(_server_fd);
			exit(EXIT_FAILURE);
		}

		for (int i = 0; i < event_count; i++)
		{
			if (events[i].data.fd == _server_fd)
			{
				int client_fd = accept(_server_fd, nullptr, nullptr);
				if (client_fd == -1)
				{
					perror("accept");
					continue;
				}
				_set_nonblocking(client_fd);
				struct epoll_event ev;
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = client_fd;
				if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
				{
					perror("epoll_ctl: client_fd");
					close(client_fd);
				}
			}
			else
				_handle_request(events[i].data.fd);
		}
	}
}
