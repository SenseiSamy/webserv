#pragma once

#include <cstddef>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <vector>

#define PORT 8080
#define MAX_EVENTS 64

class Server
{
  protected:
    struct routes_data
    {
        std::vector<std::string> methods;
        std::string redirect;
        std::string root;
        bool autoindex;
        std::string index;
        std::map<std::string, std::string> cgi;
        std::map<std::string, std::string> routes_pages;
    };

    struct server_data
    {
        std::string host;
        unsigned short port;
        std::vector<std::string> server_names;
        std::map<std::string, std::string> error_pages;
        size_t body_size;
        std::vector<routes_data> routes;

        int socket_fd;
        struct sockaddr_in addr;
        int opt;

        // Constructor with corrected member initialization order
        server_data() : socket_fd(-1), opt(1)
        {
        }
    };

    std::vector<server_data> servers;

  private:
    int _client_sock;
    struct sockaddr_in _client_addr;
    socklen_t _sin_len;
    int _epfd;
    struct epoll_event _ev;
    struct epoll_event _events[MAX_EVENTS];

    std::string _path;
    std::vector<std::vector<std::string> > _files_content;

    std::vector<std::string> split_line(const std::string &str);
    void read_files();

    /* parsing */
    int parsing_routes(const std::vector<std::string> &token_args, routes_data &new_routes,
                       const std::string &current_word, size_t line);
    int parsing_server(const std::vector<std::string> &token_args, server_data &new_server,
                       const std::string &current_word, size_t line);
    int parsing_config();

    /* syntax */
    int syntax_brackets();
    
    /* socket */
    int open_socket();

  public:
    explicit Server(const std::string &path);
    virtual ~Server();

    int run();

    void syntax_config() const;
    void parse_config();
};
