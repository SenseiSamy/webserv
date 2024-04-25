#include "Server.hpp"

#include "Request.hpp"
#include "Response.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

/* --------------- Server --------------- */
Server::Server(const std::string &path) : _path(path)
{
    if (read_files() == -1)
        throw std::runtime_error("Error: cannot open the file " + path);
    if (_file_config_content.empty())
        throw std::runtime_error("Error: config file cannot be empty");
    if (syntax_brackets() == -1)
        throw std::runtime_error("Error: syntax error in file " + path);
    if (parsing_config() == -1)
        throw std::runtime_error("Error: parsing error in file " + path);
}

Server::~Server()
{
    _file_config_content.clear();
}

int Server::handle_new_connection(int server_fd)
{
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (client_fd == -1)
        return (std::cerr << "accept failed" << std::endl, -1);

    int flags = fcntl(client_fd, F_GETFL, 0);
    if (flags == -1 || fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "fcntl failed" << std::endl;
        close(client_fd);
        return -1;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client_fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
    {
        std::cerr << "epoll_ctl: client_fd" << std::endl;
        close(client_fd);
        return -1;
    }

    return 0;
}

int Server::run()
{
    int epoll_fd = epoll_create(MAX_EVENTS);
    if (epoll_fd == -1)
        return (std::cerr << "epoll_create failed" << std::endl, -1);

    struct epoll_event ev, events[MAX_EVENTS];

    for (size_t i = 0; i < _servers.size(); ++i)
    {
        int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd == -1)
            return (std::cerr << "socket failed" << std::endl, -1);

        int flags = fcntl(listen_fd, F_GETFL, 0);
        if (fcntl(listen_fd, F_SETFL, flags | O_NONBLOCK) < 0)
            return (std::cerr << "fcntl failed" << std::endl, -1);

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(_servers[i].host.c_str());
        addr.sin_port = htons(_servers[i].port);

        if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
            return (std::cerr << "bind failed" << std::endl, -1);

        if (listen(listen_fd, SOMAXCONN) == -1)
            return (std::cerr << "listen failed" << std::endl, -1);

        ev.events = EPOLLIN;
        ev.data.fd = listen_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) == -1)
            return (std::cerr << "epoll_ctl: listen_fd" << std::endl, -1);
    }

    for (;;)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1)
            return (std::cerr << "epoll_wait failed" << std::endl, -1);

        for (int n = 0; n < nfds; ++n)
            if (events[n].events & EPOLLIN && handle_new_connection(events[n].data.fd) == -1)
                continue;
    }

    close(epoll_fd);
    return 0;
}
