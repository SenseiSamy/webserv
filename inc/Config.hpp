#pragma once

#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#define WHITESPACE " \t\n\r\f\v"
#define COMMENT "#"
#define EQUAL ":"
#define DELIMITER ","

enum Method
{
    GET,
    POST,
    DELETE
};

struct Route
{
    std::string path;                       // URL path
    std::vector<Method> allow_methods;      // Allowed HTTP methods
    std::string redirect_to;                // URL for HTTP redirection
    std::string root;                       // Directory or file path for content
    bool directory_listing;                 // Flag for directory listing
    std::string default_file;               // Default file for directory requests
    std::map<std::string, std::string> cgi; // CGI scripts mapping
    std::string upload_path;                // Path for uploaded files
};

struct Server
{
    std::vector<std::string> host;                                  // Hostnames
    unsigned short port;                                            // Listening port
    std::map<std::vector<unsigned short>, std::string> error_pages; // Custom error pages
    size_t client_max_body_size;                                    // Maximum request body size
    std::vector<Route> routes;                                      // Route configurations
};

class Config
{
  public:
    Config(const std::string &filePath);
    void parseConfigFile();
    void displayConfig() const;

    std::vector<Server> servers;

  private:
    std::string filePath;

    const std::vector<std::string> split(const std::string &s, const std::string &delim) const;

    void parseServerDirective(Server &server, std::string &line, std::ifstream &file);
    void parseRouteDirective(Route &route, std::string &line, std::ifstream &file);
};
