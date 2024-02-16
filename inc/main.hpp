#pragma once

#include <iostream>
#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h> // socket
#include <unistd.h>     // close

#define PORT 8080

class Request {
private:
  int server_fd;
  int new_socket;
  struct sockaddr_in client_addr;

public:
  Request();
  ~Request();
  int init_socket(int port);
};

class Response {

};

class CGI {

};

class Server {
private:
  Request request;
  Response response;
  CGI cgi;
};


