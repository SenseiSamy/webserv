#pragma once

#include <map>
#include <string>
#include <vector>

class Config
{
  public:
    struct RouteConfig
    {
        std::vector<std::string> methods;
        std::string httpMethod;
        std::string root;
        bool directoryListing;
        std::string defaultFile;
        std::map<std::string, std::string> cgi;
        std::string uploadPath;
    };

    struct ServerConfig
    {
        int port;
        std::string host;
        std::vector<std::string> serverNames;
        std::map<std::string, std::string> errorPages;
        int clientBodySize;
        std::map<std::string, RouteConfig> routes;
    };

    Config(const std::string &path_config) : path_config(path_config) {}
    ~Config() { }
    bool parseConfig();
    const std::vector<ServerConfig> &getServers() const;

  private:
    std::string path_config;
    std::vector<ServerConfig> servers;

    void parseServerBlock(std::istream &stream, ServerConfig &serverConfig);
    void parseRouteBlock(std::istream &stream, RouteConfig &routeConfig);
};
