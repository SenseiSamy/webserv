#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include <cerrno>
#include <cstddef>
#include <fcntl.h>
#include <netinet/in.h>
#include <ostream>
#include <sstream>

#include <csignal>
#include <iostream>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

static const std::map<unsigned short, std::string> init_error_codes()
{
	std::map<unsigned short, std::string> error_codes;

	error_codes[100] = "Continue";
	error_codes[101] = "Switching Protocols";
	error_codes[102] = "Processing";
	error_codes[200] = "OK";
	error_codes[201] = "Created";
	error_codes[202] = "Accepted";
	error_codes[203] = "Non-Authoritative Information";
	error_codes[204] = "No Content";
	error_codes[205] = "Reset Content";
	error_codes[206] = "Partial Content";
	error_codes[207] = "Multi-Status";
	error_codes[208] = "Already Reported";
	error_codes[226] = "IM Used";
	error_codes[300] = "Multiple Choices";
	error_codes[301] = "Moved Permanently";
	error_codes[302] = "Found";
	error_codes[303] = "See Other";
	error_codes[304] = "Not Modified";
	error_codes[305] = "Use Proxy";
	error_codes[306] = "(Unused)";
	error_codes[307] = "Temporary Redirect";
	error_codes[308] = "Permanent Redirect";
	error_codes[400] = "Bad Request";
	error_codes[401] = "Unauthorized";
	error_codes[402] = "Payment Required";
	error_codes[403] = "Forbidden";
	error_codes[404] = "Not Found";
	error_codes[405] = "Method Not Allowed";
	error_codes[406] = "Not Acceptable";
	error_codes[407] = "Proxy Authentication Required";
	error_codes[408] = "Request Timeout";
	error_codes[409] = "Conflict";
	error_codes[410] = "Gone";
	error_codes[411] = "Length Required";
	error_codes[412] = "Precondition Failed";
	error_codes[413] = "Payload Too Large";
	error_codes[414] = "URI Too Long";
	error_codes[415] = "Unsupported Media Type";
	error_codes[416] = "Range Not Satisfiable";
	error_codes[417] = "Expectation Failed";
	error_codes[418] = "I'm a teapot";
	error_codes[421] = "Misdirected Request";
	error_codes[422] = "Unprocessable Entity";
	error_codes[423] = "Locked";
	error_codes[424] = "Failed Dependency";
	error_codes[425] = "Too Early";
	error_codes[426] = "Upgrade Required";
	error_codes[428] = "Precondition Required";
	error_codes[429] = "Too Many _requests";
	error_codes[431] = "Request Header Fields Too Large";
	error_codes[451] = "Unavailable For Legal Reasons";
	error_codes[500] = "Internal Server Error";
	error_codes[501] = "Not Implemented";
	error_codes[502] = "Bad Gateway";
	error_codes[503] = "Service Unavailable";
	error_codes[504] = "Gateway Timeout";
	error_codes[505] = "HTTP Version Not Supported";
	error_codes[506] = "Variant Also Negotiates";
	error_codes[507] = "Insufficient Storage";
	error_codes[508] = "Loop Detected";
	error_codes[510] = "Not Extended";
	error_codes[511] = "Network Authentication Required";

	return error_codes;
}

Server::Server() : _error_codes(init_error_codes()), _current_word(0), _current_line(0)
{
}

Server::Server(const char *config_file, bool verbose)
		: _verbose(verbose), _error_codes(init_error_codes()), _config_file(config_file), _current_word(0), _current_line(0)
{
	_read_config();
	_parsing_config();
}

Server::Server(const Server &other)
{
	if (this != &other)
	{
		_verbose = other._verbose;
		_epoll_fd = other._epoll_fd;
		_servers = other._servers;
		_requests = other._requests;
		_config_file = other._config_file;
		_content_file = other._content_file;
		_current_word = other._current_word;
		_current_line = other._current_line;
	}
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_verbose = other._verbose;
		_epoll_fd = other._epoll_fd;
		_servers = other._servers;
		_requests = other._requests;
		_config_file = other._config_file;
		_content_file = other._content_file;
		_current_word = other._current_word;
		_current_line = other._current_line;
	}
	return *this;
}

Server::~Server()
{
	for (size_t i = 0; i < Server::_servers.size(); ++i)
	{
		std::cout << "Deleting server " << _servers[i].host << ":" << _servers[i].port << std::endl;
		close(_servers[i].listen_fd);
	}
}

std::string Server::to_string(size_t i) const
{
	std::ostringstream oss;
	oss << i;
	return oss.str();
}

bool Server::_accept_new_connection(server *server)
{
	sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	struct epoll_event ev;
	int client_fd = accept(server->listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_fd == -1)
	{
		std::cerr << "accept() failed " << std::string(strerror(errno));
		return (false);
	}

	fcntl(client_fd, F_SETFL, O_NONBLOCK);

	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = client_fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
	{
		close(_epoll_fd);
		throw std::runtime_error("epoll_ctl() failed " + std::string(strerror(errno)));
	}
	return (true);
}

void Server::_read_request(int fd)
{
	char buffer[MAX_BUFFER_SIZE];
	ssize_t count = read(fd, buffer, MAX_BUFFER_SIZE);
	Request* request = &_requests[fd];

	if (request->get_servers().size() == 0)
	{
		request->set_servers(_servers);
		request->set_server(request->get_servers()[0]);
	}
	_requests[fd] += std::string(buffer, count);

	if (count == 0 || count == -1)
		request->set_state(invalid);

	if (request->http100_continue)
	{
		send(fd, "HTTP/1.1 100 Continue\r\n\r\n", 25, 0);
		request->http100_continue = false;
	}
}

void Server::run()
{
	if (_verbose)
		display();
	_epoll_fd = epoll_create(1);
	if (_epoll_fd == -1)
		throw std::runtime_error("epoll_create1() failed " + std::string(strerror(errno)));

	struct epoll_event ev, events[MAX_EVENTS];
	for (size_t i = 0; i < Server::_servers.size(); ++i)
	{
		_setup_server_socket(Server::_servers[i]);

		ev.events = EPOLLIN | EPOLLOUT;
		ev.data.fd = Server::_servers[i].listen_fd;
		if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, Server::_servers[i].listen_fd, &ev) == -1)
		{
			close(_epoll_fd);
			throw std::runtime_error("epoll_ctl() failed " + std::string(strerror(errno)));
		}
	}
	std::cout << "----------------------------------------" << std::endl;

	while (true)
	{
		int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
		if (nfds < 0)
		{
			if (errno == EINTR)
				continue;
			else
				throw std::runtime_error("epoll_wait() failed " + std::string(strerror(errno)));
		}

		for (int i = 0; i < nfds; ++i) // Iterating over all the fds that are available for reading/writing
		{
			const int fd = events[i].data.fd;
			server *server = NULL;

			for (size_t j = 0; j < Server::_servers.size(); ++j)
			{
				if (Server::_servers[j].listen_fd == fd)
				{
					server = &Server::_servers[j];
					break;
				}
			}

			if (server != NULL) // Accepting a new connection
			{
				if (!_accept_new_connection(server))
					continue;
			}
			else if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) // An error happened
				close(fd);
			else if (events[i].events & EPOLLIN) // Reading client request and if complete, sending response
			{
				_read_request(fd);
				const Request &request = _requests[fd];
				if (request.get_state() != complete && request.get_state() != invalid)
					continue;
				if (_verbose)
				{
					request.display();
					std::cout << "----------------------------------------" << std::endl;
				}
				Response response;
				struct server server = request.get_server();

				if (request.get_state() == complete)
					response = Response(request, server, _error_codes);
				else if (request.get_state() == invalid)
				{
					if (request.get_file_size() > server.max_body_size) // Payload Too Large
						response = Response(413, server, _error_codes);
					else if (request.get_file_size() == 0) // Bad Request
						response = Response(400, server, _error_codes);
					else // Length Required
						response = Response(411, server, _error_codes);
				}
				if (_verbose)
					response.display();
				std::cout << server.host << ":" << server.port << " - - \"" << request.get_first_line() << "\" ";
				if (response.get_status_code() >= 100 && response.get_status_code() < 200)
					std::cout << "\033[1;34m" << response.get_status_code();
				else if (response.get_status_code() >= 200 && response.get_status_code() < 300)
					std::cout << "\033[1;32m" << response.get_status_code();
				else if (response.get_status_code() >= 300)
					std::cout << "\033[1;31m" << response.get_status_code() << " " << response.get_status_message();
				std::cout << "\033[0m -" << std::endl;
				if (_verbose)
					std::cout << "----------------------------------------" << std::endl;

				std::string response_str = response.convert();

				send(fd, response_str.c_str(), response_str.size(), 0);

				_requests[fd].clear();
				_requests.erase(fd);
				close(fd);
			}
		}
	}
	close(_epoll_fd);
}
