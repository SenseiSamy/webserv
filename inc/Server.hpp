#pragma once

#include <cstddef>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <vector>

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
    };

    std::vector<server_data> servers;

  private:
    int _epoll_fd;
    int _listen_fd;
    static const int MAX_EVENTS = 10;

    std::string _path;
    std::vector<std::vector<std::string> > _file_config_content;

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
