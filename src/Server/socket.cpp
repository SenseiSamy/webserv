#include "Server.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <ostream>
#include <sys/epoll.h>
#include <cerrno>

int Server::open_sockets()
{
    for (std::vector<server_data>::iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
        it->_listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (it->_listen_fd < 0)
            return (std::cerr << "open_sockets: socket: " << strerror(errno) << std::endl, -1);

        int optval = 1;
        if (setsockopt(it->_listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
            return (std::cerr << "open_sockets: setsockopt: " << strerror(errno) << std::endl, -1);

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(it->host.c_str());
        addr.sin_port = htons(it->port);
        memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));

        if (bind(it->_listen_fd, (struct sockaddr*)(&addr), sizeof(addr)) == -1)
            return (std::cerr << "open_sockets: bind: " << strerror(errno) << std::endl, -1);
        if (listen(it->_listen_fd, 5) == -1)
            return (std::cerr << "open_sockets: listen: " << strerror(errno) << std::endl, -1);

        std::cout << "Server started on \033[1;34m" << it->host << ":" << it->port << "\033[0m" << std::endl;
    }

    _epoll_fd = epoll_create(1);
    if (_epoll_fd == -1)
        return (std::cerr << "open_sockets: epoll_create: " << strerror(errno) << std::endl, -1);

    epoll_event event;
    event.events = EPOLLIN | EPOLLET; // Edge-triggered read events
    for (std::size_t i = 0; i < _servers.size(); ++i)
    {
        event.data.fd = _servers[i]._listen_fd;
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _servers[i]._listen_fd, &event) == -1)
            return (std::cerr << "open_sockets: epoll_ctl: " << strerror(errno) << std::endl, -1);
    }

    return 0;
}
