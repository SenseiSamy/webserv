#pragma once

#include "Request.hpp"

#include <cerrno>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct routes_data
{
    std::string root;
    std::string rewrite;
    std::vector<std::string> methods;
    std::string redirect;
    bool autoindex;
    std::string index;
    std::map<std::string, std::string> cgi;
    std::map<std::string, std::string> routes_pages;
};

struct server_data
{
    std::string host;
    unsigned short port;
    std::string root;
    std::vector<std::string> server_names;
    std::map<std::string, std::string> error_pages;
    size_t body_size;
    std::vector<routes_data> route;

    int _listen_fd;
};

class Server
{
  private:
    std::vector<server_data> _servers;
    int _epoll_fd;
    static const int MAX_EVENTS = 10;

    std::string _path;
    std::vector<std::vector<std::string> > _file_config_content;

    std::vector<std::string> split_line(const std::string& str);
    int read_files();

    /* parsing */
    int parsing_routes(const std::vector<std::string>& token_args, routes_data& new_routes,
                       const std::string& current_word, size_t line);
    int parsing_server(const std::vector<std::string>& token_args, server_data& new_server,
                       const std::string& current_word, size_t line);
    int parsing_config();

    /* syntax */
    int syntax_brackets();

    /* socket */
    int open_sockets();

    server_data* get_server_to_connect(int sock_fd);
    server_data& get_server_from_request(Request req);

    void print_log(Request& req, server_data& server) const;

  public:
    explicit Server(const std::string& path);
    ~Server();

    int run();

    void syntax_config() const;
    void parse_config();

    const std::vector<server_data> get_servers() const;
    const std::vector<std::vector<std::string> > get_file_config_content() const;
    const std::string& get_path() const;

    void display_servers() const;
    void display_file_config_content() const;
    void display_path() const;
};
