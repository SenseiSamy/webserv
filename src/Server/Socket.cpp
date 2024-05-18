#include "Server.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

void Server::setup_server_socket(server &server)
{
	server.listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (server.listen_fd == -1)
		throw std::runtime_error("socket() failed " + std::string(strerror(errno)));

	int optval = 1;
	if (setsockopt(server.listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		throw std::runtime_error("setsockopt() failed " + std::string(strerror(errno)));

	server.addr.sin_family = AF_INET;
	server.addr.sin_addr.s_addr = inet_addr(server.host.c_str());
	server.addr.sin_port = htons(server.port);
	bzero(&(server.addr.sin_zero), sizeof(server.addr.sin_zero));

	if (bind(server.listen_fd, (struct sockaddr *)&server.addr, sizeof(server.addr)) != 0 && errno != EADDRINUSE)
	{
		close(server.listen_fd);
		close(_epoll_fd);
		throw std::runtime_error("bind() failed " + std::string(strerror(errno)));
	}

	if (listen(server.listen_fd, SOMAXCONN) != 0)
	{
		close(server.listen_fd);
		close(_epoll_fd);
		throw std::runtime_error("listen() failed " + std::string(strerror(errno)));
	}

	std::cout << "Server listening on :\033[1;32m" << server.host << ":" << server.port << "\033[0m" << std::endl
	<< "\t       root :\033[1;32m" << server.root << "\033[0m" << std::endl;
}
