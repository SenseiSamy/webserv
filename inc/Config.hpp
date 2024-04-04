#pragma once

#include <cstddef>
#include <fstream>
#include <map>
#include <string>
#include <vector>

class Config
{
  private:
    struct Routes
    {
        std::vector<std::string> methods;
        std::string redirect;
        std::string root;
        bool autoindex;
        std::string index;
        std::map<std::string, std::string> cgi;
    };

    struct Server
    {
        std::string host;
        unsigned short port;
        std::vector<std::string> server_names;
        std::map<std::string, std::string> error_pages;
        size_t body_size;
        std::vector<Routes> routes;
    };

    enum Token
    {
        SERVER,
        ROUTES,
        QUOTES,
        BRACES_OPEN,
        BRACES_CLOSE,
        STRING,
        SEMICOLON,
        EQUAL,
        COMMA,
    };

    enum SyntaxError
    {
        NO_ERROR,
        BRACKETS_ERROR,
        VARIABLES_ERROR,
        SYNTAX_ERROR,
    };

    struct Tokens
    {
        std::string token;
        size_t line;
        size_t column;
        Token type;
    };

    std::string _path;
    std::ifstream _file;
    std::vector<Tokens> _tokens;
    std::vector<Server> _servers;

    int is_spearator(const char c) const;
    void tokenize();

    /* parsing */
    void parse_server(Server &server);
    void parse_routes(Server &server);

    /* syntax */
    int syntax_brackets() const;
    int syntax_variables() const;

  public:
    explicit Config(const std::string &path);
    explicit Config(const Config &other);
    ~Config();
    Config &operator=(const Config &other);

    const std::vector<Server> &getServers() const;
    const std::string &getPath() const;

    void syntax_config() const;

    void parse_config();
};
