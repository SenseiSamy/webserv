#include "Server.hpp"

#include <arpa/inet.h> // inet_addr
#include <fcntl.h>		 // fcntl, F_GETFL, F_SETFL, O_NONBLOCK
#include <iostream>		 // std::cerr, std::endl

void Server::_set_nonblocking(int sockfd)
{
	int opts = fcntl(sockfd, F_GETFL);
	if (opts < 0)
	{
		perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(sockfd, F_SETFL, opts) < 0)
	{
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
}

int Server::_bind_socket(const std::string &ip, int port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cerr << "Error: socket: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Error: setsockopt: " << strerror(errno) << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = inet_addr(ip.c_str());
	_address.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *)&_address, sizeof(_address)) == -1)
	{
		std::cerr << "Error: bind: " << strerror(errno) << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, 10) == -1)
	{
		std::cerr << "Error: listen: " << strerror(errno) << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	return sockfd;
}