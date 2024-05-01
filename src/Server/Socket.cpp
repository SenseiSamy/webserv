#include "Server.hpp"

#include <fcntl.h>
#include <iostream>

int Server::create_socket(int domain, int type, int protocol)
{
    int sock = socket(domain, type, protocol);
    if (sock == -1)
        throw std::runtime_error("socket() failed " + std::string(strerror(errno)));

    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("fcntl() failed " + std::string(strerror(errno)));

    return sock;
}

void Server::setup_server_socket(server &server)
{
    server.listen_fd = create_socket(AF_INET, SOCK_STREAM, 0);
    server.addr.sin_family = AF_INET;
    server.addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server.addr.sin_port = htons(server.port);

    if (bind(server.listen_fd, (struct sockaddr *)&server.addr, sizeof(server.addr)) != 0)
    {
        close(server.listen_fd);
        throw std::runtime_error("bind() failed " + std::string(strerror(errno)));
    }

    if (listen(server.listen_fd, SOMAXCONN) != 0)
    {
        close(server.listen_fd);
        throw std::runtime_error("listen() failed " + std::string(strerror(errno)));
    }

    std::cout << "Server listening on " << server.host << ":" << server.port << std::endl;
}