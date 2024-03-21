#include "Config.hpp"
#include "Response.hpp"
#include "logger.hpp"
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_EVENTS 64

#include <fcntl.h>

std::vector<int>	open_servers_socket(const Config& config) {
	std::vector<Config::ServerConfig> servers = config.getServers();
	std::vector<int> servers_sock;

	for (std::vector<Config::ServerConfig>::iterator it = servers.begin();
		it != servers.end(); ++it) {
		int sock = socket(AF_INET, SOCK_STREAM, SOCK_NONBLOCK);
		if (sock < 0)
        	log(FATAL, "socket error: " + std::string(strerror(errno)));

		int optval = 1;
    	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval) == -1))
			log(FATAL, "socket error: " + std::string(strerror(errno)));

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(it->host.c_str());
		addr.sin_port = htons(it->port);
		memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));

		if (bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1)
			log(FATAL, "bind error: " + std::string(strerror(errno)));
		if (listen(sock, 5) == -1)
			log(FATAL, "listen error: " + std::string(strerror(errno)));
	}
	return (servers_sock);
}

int main(void)
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
        log(FATAL, "socket error: " + std::string(strerror(errno)));

    int optval = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        log(FATAL, "bind error: " + std::string(strerror(errno)));

    listen(server_sock, 5);
    setnonblocking(server_sock);

    int epfd = epoll_create1(0);
    if (epfd == -1)
        log(FATAL, "epoll_create1 failed");

    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_sock;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_sock, &ev) == -1)
        log(FATAL, "epoll_ctl: server_sock");

    while (true)
    {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int n = 0; n < nfds; ++n)
        {
            if (events[n].data.fd == server_sock)
            {
                client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &sin_len);
                if (client_sock < 0)
                {
                    log(ERROR, "accept error: " + std::string(strerror(errno)));
                    continue;
                }
                setnonblocking(client_sock);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_sock;
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_sock, &ev) == -1)
                {
                    log(ERROR, "epoll_ctl: client_sock");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                char buffer[1024];
                ssize_t count = recv(events[n].data.fd, buffer, sizeof(buffer) - 1, 0);
                if (count == -1)
                {
                    if (errno != EAGAIN)
                    {
                        log(ERROR, "recv error: " + std::string(strerror(errno)));
                        close(events[n].data.fd);
                    }
                }
                else if (count == 0)
                    close(events[n].data.fd);
                else
                {
                    buffer[count] = '\0';
                    log(INFO, "received: " + std::string(buffer));
                    Response response(events[n].data.fd, buffer);
                    response.sendResponse();
                }
            }
        }
    }

    close(server_sock);
    return 0;
}
