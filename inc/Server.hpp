#ifndef SERVER_HPP
#define SERVER_HPP

#include "HTTPRequest.hpp"
#include <cstddef>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <vector>
#include <arpa/inet.h>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <pthread.h>
#include <string>
#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iomanip>

#define HEADER_COLOR "\033[1;34m"
#define REQUEST_COLOR "\033[1;32m"
#define LINE_NUMBER_COLOR "\033[1;33m"
#define RESET_COLOR "\033[0m"

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
    std::vector<routes_data> route;

    int _listen_fd;
};

class Server
{
  private:
    std::vector<server_data> servers;
    int _epoll_fd;
    static const int MAX_EVENTS = 10;

    std::string _path;
    std::vector<std::vector<std::string> > _file_config_content;

    std::vector<std::string> split_line(const std::string &str);
    int read_files();

    /* parsing */
    int parsing_routes(const std::vector<std::string> &token_args, routes_data &new_routes,
                       const std::string &current_word, size_t line);
    int parsing_server(const std::vector<std::string> &token_args, server_data &new_server,
                       const std::string &current_word, size_t line);
    int parsing_config();

    /* syntax */
    int syntax_brackets();

    /* socket */
    int open_sockets();

    server_data *get_server_to_connect(int sock_fd);
    server_data &get_server_from_request(HTTPRequest req);

	void print_log(HTTPRequest &req, server_data &server) const;

  public:
    explicit Server(const std::string &path);
    virtual ~Server();

    int run();

    void syntax_config() const;
    void parse_config();
};

#endif // !SERVER_HPP
