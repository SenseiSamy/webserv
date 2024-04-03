#pragma once

#include <cstddef>
#include <fstream>
#include <map>
#include <string>
#include <vector>

class Config
{
  private:
    struct RoutesData
    {
        std::vector<std::string> methods;
        std::string redirect;
        std::string root;
        bool autoindex;
        std::string index;
        std::map<std::string, std::string> cgi;
    };

    struct ServerData
    {
        std::string host;
        unsigned short port;
        std::vector<std::string> server_names;
        std::map<std::string, std::string> error_pages;
        size_t body_size;
        std::vector<RoutesData> routes;
    };

    std::string _path;
    std::ifstream _file;
    std::vector<std::string> _tokens;
    std::vector<ServerData> _servers;

    void tokenize();

    /* parsing */
    void parseServer(ServerData &server);
    void parseRoutes(ServerData &server);

    /* syntax */
    void syntaxServer(ServerData &server) const;
    void syntaxRoutes(RoutesData &route) const;

  public:
    explicit Config(const std::string &path);
    explicit Config(const Config &other);
    ~Config();
    Config &operator=(const Config &other);

    const std::vector<ServerData> &getServers() const;
    const std::string &getPath() const;

    void syntaxConfig() const;

    void parseConfig();
};
