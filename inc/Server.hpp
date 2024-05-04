#ifndef SERVER_HPP
#define SERVER_HPP

// types
#include <cstddef>
#include <map>
#include <string>
#include <sys/types.h>
#include <vector>

// fonctions
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <ostream>
#include <unistd.h>

#ifndef MAX_EVENTS
#define MAX_EVENTS 64
#endif

typedef struct route
{
    // required
    std::string path; // /kapouet

    // optional
    std::vector<std::string> accepted_methods; // GET
    std::string root;                          // /tmp/www
    std::string file_path;                     // /var/www/html
    bool directory_listing;                    // true
    std::string default_file;                  // index.html

    /// cgi
    std::map<std::string, std::string> cgi; // .php /usr/bin/php-cgi
    std::string cgi_upload_path;            // /tmp/upload
    bool cgi_upload_enable;                 // true
} route;


// TODO:
// - add default_file
typedef struct server
{
    struct sockaddr_in addr;
    int listen_fd;

    // required
    std::string host; // 127.0.0.1
    ssize_t port;     // 8080

    // optional
    std::string root;                               // /var/www/html
    std::string server_names;                       // example.com
    bool default_server;                            // true
    size_t max_body_size;                           // 1000000
    std::map<std::string, std::string> error_pages; // 404 /404.html
    std::vector<route> routes;                      // /kapouet
} server;

class Server
{
  private:
    std::string _config_file;
    std::map<size_t, std::vector<std::string> > _content_file;
    size_t _current_word;
    size_t _current_line;

    std::vector<server> _servers;

    // Index management
    void reset_index();
    const std::string next_word();
    const std::string previous_word();

    const std::vector<std::string> get_value(const std::string &token);

    // Socket management
    int create_socket(int domain, int type, int protocol);
    void setup_server_socket(server &server);

    // Parsing
    void read_config();
    const server parse_server();
    const route parse_route();

  public:
    Server();
    Server(const char *config_file);
    Server(const Server &other);
    ~Server();
    Server &operator=(const Server &other);

    // Getters
    std::vector<route> get_route(const size_t i) const;
    std::vector<server> get_servers() const;

    // Setters
    void set_servers(std::vector<server> servers);

    // Displays
    void display() const;

    // Others
    std::string to_string(size_t i) const;

    /// Syntax
    void syntax_brackets();

    /// Parsing

    void parsing_config();

    void run();
};

#endif // SERVER_HPP