#pragma once

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

#define PORT 8080
#define MAX_EVENTS 64

int parsing_reception(int client_sock);

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
