#pragma once

#include <cerrno>       // errno
#include <cstdlib>      // exit
#include <cstring>      // strlen, memset
#include <iostream>     // std::cerr, std::cout
#include <netinet/in.h> // sockaddr_in, htons/htonl
#include <sys/socket.h> // socket
#include <sys/types.h>  // socket
#include <unistd.h>     // close, write
#include <strings.h>    // bzero
#include <stdio.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <string>

#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

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
