#include "Server.hpp"
#include "Utils.hpp"

/* Functions */
#include <cerrno>		// errno
#include <cstring>	// strerror
#include <iostream> // std::cerr, std::endl
#include <unistd.h> // close

Server::Server(const server &serv)
		: _error_codes(init_error_codes()), _methods_map({{"GET", &Server::_get},
																											{"HEAD", &Server::_head},
																											{"POST", &Server::_post},
																											{"PUT", &Server::_put},
																											{"DELETE", &Server::_delete},
																											{"CONNECT", &Server::_connect},
																											{"OPTIONS", &Server::_options},
																											{"TRACE", &Server::_trace}}),
			_server(serv)
{
	if (setup_socket())
		exit(errno);
}

Server::~Server()
{
	if (_client_fd != -1)
		close(_client_fd);
	std::cout << _server.host << ":" << _server.port << " - - Server destructor" << std::endl;
}

int Server::setup_socket()
{
	_client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_client_fd == -1)
	{
		std::cerr << "Error: socket: " << strerror(errno) << std::endl;
		return errno;
	}

	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(_server.port);

	int reuse = 1;
	if (setsockopt(_client_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
	{
		std::cerr << "Error: setsockopt: " << strerror(errno) << std::endl;
		return errno;
	}

	if (bind(_client_fd, (sockaddr *)&_addr, sizeof(_addr)) == -1)
	{
		std::cerr << "Error: bind: " << strerror(errno) << std::endl;
		return errno;
	}

	if (listen(_client_fd, 100) == -1)
	{
		std::cerr << "Error: listen: " << strerror(errno) << std::endl;
		return errno;
	}

	return 0;
}