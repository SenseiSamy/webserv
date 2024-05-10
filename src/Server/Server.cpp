#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include <cerrno>
#include <cstddef>
#include <fcntl.h>
#include <netinet/in.h>
#include <sstream>

#include <iostream>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

Server::Server() : _current_word(0), _current_line(0)
{
}

Server::Server(const char *config_file) : _config_file(config_file), _current_word(0), _current_line(0)
{
	read_config();
	// reset_index();
	parsing_config();
}

Server::Server(const Server &other)
{
	if (this != &other)
	{
		_config_file = other._config_file;
		_content_file = other._content_file;
		_current_word = other._current_word;
		_current_line = other._current_line;
		_servers = other._servers;
	}
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_config_file = other._config_file;
		_content_file = other._content_file;
		_current_word = other._current_word;
		_current_line = other._current_line;
		_servers = other._servers;
	}
	return *this;
}

Server::~Server()
{
	for (size_t i = 0; i < _servers.size(); ++i)
		close(_servers[i].listen_fd);
}

std::string Server::to_string(size_t i) const
{
	std::ostringstream oss;
	oss << i;
	return oss.str();
}

const server &Server::find_server(Request &request)
{
  const std::string &host = request.get_header_key("Host");
  if (host.empty())
    return _servers[0];

  std::string::size_type sep = host.find(':');
  if (sep == std::string::npos)
    return _servers[0];

  std::stringstream ss(host.substr(sep + 1));
  ss >> sep;
  std::string hostname = host.substr(0, sep);

  std::vector<server>::iterator it = _servers.begin();
  while (it != _servers.end())
  {
    if (it->host == hostname && it->port == sep)
      return *it;
    ++it;
  }

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
  return _servers[0];
}

void Server::run()
{
	int epoll_fd = epoll_create(1);
	if (epoll_fd == -1)
		throw std::runtime_error("epoll_create1() failed " + std::string(strerror(errno)));

	struct epoll_event ev, events[MAX_EVENTS];
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		setup_server_socket(_servers[i]);

		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = _servers[i].listen_fd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _servers[i].listen_fd, &ev) == -1)
		{
			close(epoll_fd);
			throw std::runtime_error("epoll_ctl() failed " + std::string(strerror(errno)));
		}
	}

	while (true)
	{
		int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (nfds < 0)
			throw std::runtime_error("epoll_wait() failed " + std::string(strerror(errno)));

		for (int i = 0; i < nfds; ++i)
		{
			const int fd = events[i].data.fd;

			server *server;

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
				sockaddr_in client_addr;
				socklen_t client_addr_len = sizeof(client_addr);
				int client_fd = accept(server->listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);
				if (client_fd == -1)
					throw std::runtime_error("accept() failed " + std::string(strerror(errno)));

				fcntl(client_fd, F_SETFL, O_NONBLOCK);

				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = client_fd;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
				{
					close(epoll_fd);
					throw std::runtime_error("epoll_ctl() failed " + std::string(strerror(errno)));
				}
			}
			else if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))
			{
				close(fd);
				continue;
			}
			else
			{
				std::string request_str;

				char buffer[MAX_BUFFER_SIZE];
				ssize_t count;
				while ((count = read(fd, buffer, MAX_BUFFER_SIZE)) > 0)
				{
					buffer[count] = '\0';
					request_str.append(buffer, count);
				}
				if (count == -1 && errno != EAGAIN)
				{
					close(fd);
					throw std::runtime_error("read() failed " + std::string(strerror(errno)));
				}

				Request request(request_str, server->host, server->port);
				struct server server = find_server(request);
				Response response(request, server);
				send(fd, response.to_string().c_str(), response.to_string().size(), 0);
				close(fd);
			}
		}
	}
	close(epoll_fd);
}
