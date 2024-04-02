#include "Config.hpp"
#include "Response.hpp"
#include "logger.hpp"
#include <cstddef>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_EVENTS 64

#include <fcntl.h>

static void open_servers_socket(std::vector<Server>& servers) {
	for (std::vector<Server>::iterator it = servers.begin();
		it != servers.end(); ++it) {
		it->socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		if (it->socket_fd < 0)
			log(FATAL, "open_servers_socket: socket: " + std::string(strerror(errno)));

		int optval = 1;
		if (setsockopt(it->socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
			log(FATAL, "open_servers_socket: setsockopt: " + std::string(strerror(errno)));

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(it->host.c_str());
		addr.sin_port = htons(it->port);
		memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));

		if (bind(it->socket_fd, (struct sockaddr *)(&addr), sizeof(addr)) == -1)
			log(FATAL, "open_servers_socket: bind: " + std::string(strerror(errno)));
		if (listen(it->socket_fd, 5) == -1)
			log(FATAL, "open_servers_socket: listen: " + std::string(strerror(errno)));
	}
}

Server* getServer(int sock_fd, std::vector<Server>
	&servers) {
	for (std::size_t i = 0; i < servers.size(); ++i) {
		if (sock_fd == servers[i].socket_fd)
			return (&servers[i]);
	}
	return (NULL);
}

//tmp for testing
Config::Config(void) {
	Server server;

	server.port = 4934;
	server.host = "127.0.0.1";
	this->servers.push_back(server);

	server.port = 36734;
	server.host = "127.53.86.3";
	this->servers.push_back(server);
}

int main(/*int argc, const char *argv[]*/)
{
    // if (argc != 2)
    // {
    //     std::cout << "Usage: " << argv[0] << " <config_file>" << std::endl;
    //     return 1;
    // }
    Config config;

	int client_sock;
	struct sockaddr_in client_addr;
	socklen_t sin_len = sizeof(client_addr);

	open_servers_socket(config.servers);

	int epfd = epoll_create1(0);
	if (epfd == -1)
		log(FATAL, "epoll_create1 failed");

	struct epoll_event ev, events[MAX_EVENTS];
	ev.events = EPOLLIN;
	for (std::size_t i = 0; i < config.servers.size(); ++i) {
		ev.data.fd = config.servers[i].socket_fd;
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, config.servers[i].socket_fd, &ev) == -1)
			log(FATAL, "epoll_ctl: server_sock");
	}

	std::cout << "\e[0;32mServer(s) started at:\n\e[0m";
	for (std::size_t i = 0; i < config.servers.size(); ++i)
		std::cout << config.servers[i].host << ":" << config.servers[i].port << "\n";

	while (true)
	{
		int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		for (int n = 0; n < nfds; ++n)
		{
			const int fd = events[n].data.fd;
			Server* server = getServer(fd, config.servers);
			if (server != NULL) {
				client_sock = accept(server->socket_fd, (struct sockaddr *)&client_addr, &sin_len);
				if (client_sock < 0) {
					log(ERROR, "accept error: " + std::string(strerror(errno)));
					continue;
				}
				fcntl(client_sock, F_SETFL, O_NONBLOCK);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = client_sock;
				if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_sock, &ev) == -1) {
					log(ERROR, "epoll_ctl: client_sock");
					exit(EXIT_FAILURE);
				}
			}
			else if ((events[n].events & EPOLLERR) ||
				(events[n].events & EPOLLHUP) ||
				(!(events[n].events & EPOLLIN))) {
				close(fd);
			}
			else {
				std::string request("");
				char buffer[1024];
				ssize_t count;
				while ((count = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
					buffer[count] = '\0';
					request += buffer;
				}
				if (count == -1 && errno != EAGAIN) {
					log(ERROR, "recv error: " + std::string(strerror(errno)));
					close(fd);
				}
				else {
					log(INFO, "received: " + std::string(request.c_str()));
					HTTPRequest req(request);
					Response rep(req);
					send(client_sock, rep.toString().c_str(), rep.toString().size(), 0);
				}
			}
		}
	}
	return 0;
}
