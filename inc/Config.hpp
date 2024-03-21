#pragma once

#include <map>
#include <string>
#include <tuple>
#include <vector>

struct LocationConfig;
struct ServerConfig;

class Config
{
  public:
    struct LocationConfig
    {
        std::vector<std::string> limit_except;     // To represent accepted HTTP methods
        std::string return_directive;              // HTTP redirection
        std::string root;                          // Root directory
        bool autoindex;                            // Directory listing
        std::string index;                         // Default file for directories
        std::string fastcgi_pass;                  // Execute CGI
        std::string client_max_body_size;          // Limit client body size per location
        std::string fastcgi_param_SCRIPT_FILENAME; // Full path for CGI
    };

    struct ServerConfig
    {
        std::tuple<std::string, std::string> listen;     // Format: "host:port"
        std::vector<std::string> server_name;            // Server names
        std::map<std::string, std::string> error_page;   // Error pages
        std::string client_max_body_size;                // Limit client body size for the server
        std::map<std::string, LocationConfig> locations; // Key: URI location    };
    };

    explicit Config(const std::string &path_config) : _path_config(path_config)
    {
    }
    ~Config()
    {
    }
    bool parseConfig();
    const std::vector<ServerConfig> &getServers() const;

    void displayConfig() const;

  private:
    std::string _path_config;
    std::vector<ServerConfig> _servers;

    void parseServerBlock(std::istream &stream, ServerConfig &serverConfig);
    void parseLocationBlock(std::istream &stream, LocationConfig &routeConfig);
};
