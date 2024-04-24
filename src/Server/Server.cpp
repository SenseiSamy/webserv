#include "Server.hpp"

#include "Request.hpp"
#include "Response.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>

/* --------------- Server --------------- */
Server::Server(const std::string& path) : _path(path)
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

server_data* Server::get_server_to_connect(int sock_fd)
{
    for (std::size_t i = 0; i < _servers.size(); ++i)
    {
        if (sock_fd == _servers[i]._listen_fd)
            return (&_servers[i]);
    }
    return (NULL);
}

// Helper function to generate and send HTTP response
void send_response(int client_sock, const Request& request)
{
    Response response(request, server_data());
    std::string response_data = response.serialize();

    write(client_sock, response_data.c_str(), response_data.size());
    close(client_sock);
}

int Server::run()
{
    epoll_event ev, events[MAX_EVENTS];
    int client_sock;
    sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    open_sockets();

    while (true)
    {
        int num_events = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
        if (num_events < 0)
            return (-1);

        for (int n = 0; n < num_events; ++n)
        {
            const int fd = events[n].data.fd;
            server_data* server = get_server_to_connect(fd);
            if (server != NULL)
            {
                client_sock = accept(server->_listen_fd, (struct sockaddr*)&client_addr, &client_addr_size);
                if (client_sock < 0)
                {
                    std::cerr << "run: accept: " << strerror(errno) << std::endl;
                    continue;
                }
                fcntl(client_sock, F_SETFL, O_NONBLOCK);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_sock;
                if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_sock, &ev) == -1)
                {
                    std::cerr << "run: epoll_ctl: " << strerror(errno) << std::endl;
                    return (-1);
                }
            }
            else if ((events[n].events & EPOLLERR) || (events[n].events & EPOLLHUP) || (!(events[n].events & EPOLLIN)))
                close(fd);
            else
            {
                std::string request("");
                char buffer[1024];
                ssize_t count;
                while ((count = read(fd, buffer, sizeof(buffer) - 1)) > 0)
                {
                    buffer[count] = '\0';
                    request.append(buffer, count);
                }
                if (count == -1 && errno != EAGAIN)
                {
                    std::cerr << "run: read: " << strerror(errno) << std::endl;
                    close(fd);
                }
                else
                {
                    std::string request_data("");
                    char buffer[1024];
                    ssize_t count;
                    while ((count = read(fd, buffer, sizeof(buffer) - 1)) > 0)
                    {
                        buffer[count] = '\0';
                        request_data.append(buffer, count);
                    }
                    if (count == -1 && errno != EAGAIN)
                    {
                        std::cerr << "run: read: " << strerror(errno) << std::endl;
                        close(fd);
                    }
                    else if (!request_data.empty())
                    {
                        Request req(request_data);
                        send_response(fd, req);
                    }
                }
            }
        }
    }
    return 0;
}