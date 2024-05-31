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

volatile sig_atomic_t g_signal_status = 0;

static inline const std::map<unsigned short, std::string> init_error_codes()
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
	error_codes[429] = "Too Many Requests";
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

bool Server::_stop_server = false;

Server::Server() : _current_word(0), _current_line(0), _error_codes(init_error_codes())
{
}

Server::Server(const char *config_file, const bool verbose)
		: _verbose(verbose), _config_file(config_file), _current_word(0), _current_line(0), _error_codes(init_error_codes())
{
	read_config();
	parsing_config();
	signal(SIGINT, Server::signal_handler);
}

Server::Server(const Server &other)
{
	if (this != &other)
	{
		_verbose = other._verbose;
		_epoll_fd = other._epoll_fd;
		_config_file = other._config_file;
		_content_file = other._content_file;
		_current_word = other._current_word;
		_current_line = other._current_line;
		_stop_server = other._stop_server;
		_servers = other._servers;
		_requests = other._requests;
	}
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_verbose = other._verbose;
		_epoll_fd = other._epoll_fd;
		_config_file = other._config_file;
		_content_file = other._content_file;
		_current_word = other._current_word;
		_current_line = other._current_line;
		_stop_server = other._stop_server;
		_servers = other._servers;
		_requests = other._requests;
	}
	return *this;
}

Server::~Server()
{
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		std::cout << "Closing server " << _servers[i].host << ":" << _servers[i].port << std::endl;
		close(_servers[i].listen_fd);
	}
}

const server &Server::find_server(const std::string &host)
{
	if (host.empty())
		return _servers[0];

	std::vector<server>::iterator it;
	std::string hostname;
	std::string::size_type sep = host.find(':');
	if (sep != std::string::npos)
	{
		std::stringstream ss(host.substr(sep + 1));
		hostname = host.substr(0, sep);
		ss >> sep;

		it = _servers.begin();
		while (it != _servers.end())
		{
			if (it->host == hostname && it->port == sep)
				return *it;
			++it;
		}
	}
	else
		hostname = host;

	for (it = _servers.begin(); it != _servers.end(); ++it)
	{
		std::vector<std::string>::iterator name_it = it->server_names.begin();
		while (name_it != it->server_names.end())
		{
			if (*name_it == hostname)
				return *it;
			++name_it;
		}
	}

	for (it = _servers.begin(); it != _servers.end(); ++it)
		if (it->default_server)
			return *it;

	return _servers[0];
}

void Server::signal_handler(int signum)
{
	std::cout << "Caught signal " << signum << std::endl;
	_stop_server = true;
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

	ev.events = EPOLLIN | EPOLLET;
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
	while (count > 0)
	{ 
		requests[fd] += std::string(buffer, count);
		count = read(fd, buffer, MAX_BUFFER_SIZE);
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
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		setup_server_socket(_servers[i]);

		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = _servers[i].listen_fd;
		if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _servers[i].listen_fd, &ev) == -1)
		{
			close(_epoll_fd);
			throw std::runtime_error("epoll_ctl() failed " + std::string(strerror(errno)));
		}
	}
	std::cout << "----------------------------------------" << std::endl;

	while (!_stop_server)
	{
		int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
		if (nfds < 0)
		{
			if (errno == EINTR)
				continue;
			else
				throw std::runtime_error("epoll_wait() failed " + std::string(strerror(errno)));
		}

		for (int i = 0; i < nfds; ++i) 
		{
			const int fd = events[i].data.fd;
			server *server = NULL;

			for (size_t j = 0; j < _servers.size(); ++j)
			{
				if (_servers[j].listen_fd == fd)
				{
					server = &_servers[j];
					break;
				}
			}

			if (server != NULL) 
			{
				if (!_accept_new_connection(server))
					continue;
			}
			else if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) ||
							 (!(events[i].events & EPOLLIN)))
			{
				close(fd);
				continue;
			}
			else // Reading client request and if complete, sending response
			{

				_read_request(fd);	
				const Request& request = requests[fd];
				if (request.get_state() != Request::complete &&
					request.get_state() != Request::invalid)
					continue;
				if (_verbose)
				{
					request.display();
					std::cout << "----------------------------------------" << std::endl;
				}
				const struct server server = find_server(request.get_headers_key("Host"));

				Response response;
				if (request.get_state() == Request::complete)
					response = Response(request, server, _error_codes);
				else
					response = Response(400, server, _error_codes);
				if (_verbose)
					response.display();
				std::cout << server.host << ":" << server.port << " - - \"" << request.get_first_line() << "\" ";
				if (response.get_status_code() == 200)
					std::cout << "\033[1;32m" << response.get_status_code();
				else
					std::cout << "\033[1;31m" << response.get_status_code() << " " << response.get_status_message();
				std::cout << "\033[0m -" << std::endl;
				if (_verbose)
					std::cout << "----------------------------------------" << std::endl;

				std::string response_str = response.convert();

				send(fd, response_str.c_str(), response_str.size(), 0);

				requests[fd].clear();
				requests.erase(fd);
				close(fd);
			}
		}
	}
	close(_epoll_fd);
}
