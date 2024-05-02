#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include <cstddef>
#include <sstream>

#include <iostream>
#include <sys/epoll.h>

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

void Server::run()
{
    int epoll_fd = epoll_create1(0);
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
        if (nfds == -1)
        {
            if (errno == EINTR)
                continue;
            throw std::runtime_error("epoll_wait() failed " + std::string(strerror(errno)));
        }

        for (int i = 0; i < nfds; ++i)
        {
            for (size_t j = 0; j < _servers.size(); ++j)
            {
                if (events[i].data.fd == _servers[j].listen_fd)
                {
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    int client_fd = accept(_servers[j].listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                    if (client_fd == -1)
                        throw std::runtime_error("accept() failed " + std::string(strerror(errno)));

                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
                    {
                        close(epoll_fd);
                        throw std::runtime_error("epoll_ctl() failed " + std::string(strerror(errno)));
                    }
                }
                else
                {
                    char buffer[4096];
                    ssize_t bytes_read = read(events[i].data.fd, buffer, sizeof(buffer));
                    if (bytes_read == -1)
                        throw std::runtime_error("read() failed " + std::string(strerror(errno)));

                    Request request(events[i].data.fd, std::string(buffer, bytes_read));
                    request.display();
                    Response response(request, _servers[j]);
                    response.send_response();
                }
            }
        }
    }
    close(epoll_fd);
}
