#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include <cstddef>
#include <fstream>
#include <sstream>

#include <iostream>
#include <sys/epoll.h>

Server::Server() : _current_word(0), _current_line(0)
{
}

static const std::vector<std::string> split_line(const std::string &str)
{
    std::vector<std::string> words;
    std::string word;
    bool in_quote = false;

    for (std::string::size_type i = 0; i < str.size(); ++i)
    {
        if (str[i] == '"' || str[i] == '\'')
            in_quote = !in_quote;

        if (in_quote)
        {
            word += str[i];
            continue;
        }

        if (std::isspace(str[i]) || str[i] == '=' || str[i] == ';' || str[i] == '{' || str[i] == '}')
        {
            if (!word.empty())
            {
                words.push_back(word);
                word.clear();
            }
            if (!std::isspace(str[i]))
                words.push_back(std::string(1, str[i]));
        }
        else
            word += str[i];
    }
    if (!word.empty())
        words.push_back(word);
    return words;
}

Server::Server(const char config_file[]) : _current_word(0), _current_line(0)
{
    std::ifstream file(config_file);
    // if (!file.is_open())
    //     throw std::runtime_error("Failed to open file : " + std::string(strerror(errno)));

    size_t line_number = 0;
    std::string line;
    while (std::getline(file, line))
    {
        ++line_number;
        // Remove comments
        std::string::size_type end = line.find('#');
        if (end != std::string::npos)
            line = line.substr(0, end);

        if (line.empty())
            continue;

        std::vector<std::string> parsed_line = split_line(line);
        if (parsed_line.empty())
            continue;

        _content_file[line_number] = parsed_line;
    }

    file.close();
    reset_index();

    parsing_config();
}

Server::Server(const Server &other)
{
    if (this != &other)
    {
        _current_word = other._current_word;
        _current_line = other._current_line;
        _servers = other._servers;
        _content_file = other._content_file;
    }
}

Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {
        _current_word = other._current_word;
        _current_line = other._current_line;
        _servers = other._servers;
        _content_file = other._content_file;
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
                    Response response(request, _servers[j]);
                    response.send_response();
                }
            }
        }
    }
    close(epoll_fd);
}
