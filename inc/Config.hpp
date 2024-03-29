#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#define WHITE_SPACE " \t\n\r\f\v"
#define START_BLOCK "{"
#define END_BLOCK "}"
#define COMMENT "#"
#define END_LINE ";"

struct Location
{
    std::vector<std::string> limit_except;       // list of accepted HTTP methods for the route
    std::pair<std::string, std::string> rewrite; // HTTP redirection
    std::string root;                            // root directory
    bool autoindex;                              // directory listing
    std::string index;                           // default file
    std::map<std::string, std::string> cgi;      // map of CGI paths to executables
};
struct Server
{
    std::pair<std::string, std::uint16_t> listen; // host, port
    std::vector<std::string> server_names;        // server names
    std::map<int, std::string> error_pages;       // map status codes to URI paths
    std::size_t client_max_body_size;             // size in bytes
    std::map<std::string, Location> locations;    // map of URI paths to Location
	int socket_fd;
};

class Config
{
  public:
    Config(const std::string &config_file) : config_file(config_file)
    {
    }
    ~Config();

    Config(const Config &other)
    {
        *this = other;
    }
    Config &operator=(const Config &other)
    {
        if (this != &other)
            servers = other.servers;
        return *this;
    }

    const std::vector<Server> &getServers() const
    {
        return servers;
    }

    void parseConfig();

  private:
    std::string config_file;
    std::vector<Server> servers;

    int stringToInt(const std::string &str);

    std::string parseToken(const std::string &token_name, std::string &line);
    void parseServer(std::ifstream &file, Server &server);
    void parseLocation(std::ifstream &file, Location &location);
};
