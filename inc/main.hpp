#pragma once

#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include "logger.hpp"
#include "Response.hpp"

#define PORT 8080

int	parsing_reception(int client_sock, logger log);

/*
class Request
{
  private:
    int server_fd;
    int new_socket;
    struct sockaddr_in client_addr;

  public:
    Request() : server_fd(-1), new_socket(-1)
    {
    }
    Request(int server_fd) : server_fd(server_fd), new_socket(-1)
    {
    }
    Request(const Request &other)
    {
        *this = other;
    }
    ~Request()
    {
        close(server_fd);
    }

    Request &operator=(const Request &other)
    {
        server_fd = other.server_fd;
        new_socket = other.new_socket;
        client_addr = other.client_addr;
        return *this;
    }

    int init_socket(int port);
};

class Response
{

};

class CGI
{
};

class Server
{
  private:
    Request request;
    Response response;
    CGI cgi;
};
*/
