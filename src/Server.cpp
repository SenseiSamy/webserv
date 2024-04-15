#include "Server.hpp"
#include "Response.hpp"
#include <arpa/inet.h>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <string>
#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>

/* --------------- Server --------------- */
Server::Server(const std::string &path) : _path(path)
{
    read_files();

    if (_file_config_content.empty())
        throw std::runtime_error("Error: could not open file " + path);

    if (syntax_brackets() == -1)
        throw std::runtime_error("Error: syntax error in file " + path);

    if (parsing_config() == -1)
        throw std::runtime_error("Error: parsing error in file " + path);
}

Server::~Server()
{
    _file_config_content.clear();
}

/* -------------------------------------- */

std::vector<std::string> Server::split_line(const std::string &str)
{
    std::vector<std::string> words;
    std::string word;
    bool in_quote = false;

    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '"' || str[i] == '\'')
            in_quote = !in_quote;

        if (in_quote)
        {
            word += str[i];
            continue;
        }

        if (std::isspace(str[i]) || str[i] == '=' || str[i] == ';' || str[i] == '{' || str[i] == '}')
        {
            if (!word.empty())
            {
                words.push_back(word);
                word.clear();
            }
            if (!std::isspace(str[i]))
                words.push_back(std::string(1, str[i]));
        }
        else
        {
            word += str[i];
        }
    }
    if (!word.empty())
        words.push_back(word);
    return words;
}

void Server::read_files()
{
    std::ifstream file(_path.c_str());
    if (!file.is_open() || !file.good())
        std::cerr << "Error: could not open file " << _path << std::endl;

    std::string line;
    while (std::getline(file, line))
    {
        // Remove comments
        std::string::size_type end = line.find('#');
        if (end != std::string::npos)
            line = line.substr(0, end);

        if (line.empty())
            continue;

        std::vector<std::string> parsed_line = split_line(line);
        if (parsed_line.empty())
            continue;

        _file_config_content.push_back(parsed_line);
    }

    file.close();
}

int Server::syntax_brackets()
{
    size_t open_brackets = 0;
    size_t close_brackets = 0;
    std::string last_word;

    for (size_t line = 0; line < _file_config_content.size(); ++line)
    {
        for (size_t i = 0; i < _file_config_content[line].size(); ++i)
        {
            // last word
            if (i > 0)
                last_word = _file_config_content[line][i - 1];
            else if (line > 0)
                last_word = _file_config_content[line - 1].back();
            std::string word = _file_config_content[line][i];
            if (word == "{")
            {
                ++open_brackets;
                if (last_word != "server" && last_word != "routes")
                    return (std::cerr << "Syntax error: Unexpected '{' at line " << line + 1 << "." << std::endl, -1);
            }
            else if (word == "}")
                ++close_brackets;
        }
    }

    if (open_brackets != close_brackets)
        return (std::cerr << "Syntax error: Unbalanced brackets." << std::endl, -1);

    return 0;
}

int Server::parsing_routes(const std::vector<std::string> &token_args, routes_data &new_routes,
                           const std::string &current_word, size_t line)
{
    if (current_word == "methods")
    {
        for (size_t j = 0; j < token_args.size(); ++j)
        {
            if (token_args[j] != "GET" && token_args[j] != "POST" && token_args[j] != "DELETE")
                return (std::cerr << "Syntax error: Invalid method at line: " << line + 1 << "." << std::endl, -1);
        }
        new_routes.methods = token_args;
    }
    else if (current_word == "redirect")
    {
        if (token_args.size() != 1)
            return (std::cerr << "Syntax error: Invalid redirect at line: " << line + 1 << "." << std::endl, -1);

        new_routes.redirect = token_args[0];
    }
    else if (current_word == "root")
    {
        if (token_args.size() != 1)
            return (std::cerr << "Syntax error: Invalid root at line: " << line + 1 << "." << std::endl, -1);

        new_routes.root = token_args[0];
    }
    else if (current_word == "autoindex")
    {
        if (token_args.size() != 1)
            return (std::cerr << "Syntax error: Invalid autoindex at line: " << line + 1 << "." << std::endl, -1);
        if (token_args[0] != "on" && token_args[0] != "off")
            return (std::cerr << "Syntax error: Invalid autoindex value at line: " << line + 1 << "." << std::endl, -1);
        new_routes.autoindex = token_args[0] == "on";
    }
    else if (current_word == "default_index")
    {
        if (token_args.size() != 1)
            return (std::cerr << "Syntax error: Invalid index at line: " << line + 1 << "." << std::endl, -1);
        new_routes.index = token_args[0];
    }
    else if (current_word == "cgi")
    {
        if (token_args.size() != 2)
            return (std::cerr << "Syntax error: Invalid cgi at line: " << line + 1 << "." << std::endl, -1);
        new_routes.cgi[token_args[0]] = token_args[1];
    }
    else if (current_word == "routes_pages")
    {
        if (token_args.size() != 2)
            return (std::cerr << "Syntax error: Invalid routes_pages at line: " << line + 1 << "." << std::endl, -1);

        for (size_t j = 0; j < token_args[0].size(); ++j)
        {
            if (!std::isdigit(token_args[0][j]))
                return (std::cerr << "Syntax error: Invalid routes_pages value at line: " << line + 1 << "."
                                  << std::endl,
                        -1);
        }

        new_routes.routes_pages[token_args[0]] = token_args[1];
    }
    else
        return (std::cerr << "Syntax error: Invalid keyword at line: " << line + 1 << "." << std::endl, -1);
    return 0;
}

int Server::parsing_server(const std::vector<std::string> &token_args, server_data &new_server,
                           const std::string &current_word, size_t line)
{
    if (current_word == "port")
    {
        if (token_args.size() != 1)
            return (std::cerr << "Syntax error: Invalid port at line: " << line + 1 << "." << std::endl, -1);
        for (size_t j = 0; j < token_args[0].size(); ++j)
        {
            if (!std::isdigit(token_args[0][j]))
                return (std::cerr << "Syntax error: Invalid port value at line: " << line + 1 << "." << std::endl, -1);
        }
        std::istringstream iss(token_args[0]);
        iss >> new_server.port;
    }
    else if (current_word == "host")
    {
        if (token_args.size() < 1)
            return (std::cerr << "Syntax error: Invalid host at line: " << line + 1 << "." << std::endl, -1);

        new_server.host = token_args[0];
    }
    else if (current_word == "server_names")
    {
        if (token_args.size() < 1)
            return (std::cerr << "Syntax error: Invalid server_names at line: " << line + 1 << "." << std::endl, -1);

        new_server.server_names = token_args;
    }
    else if (current_word == "error_pages")
    {
        if (token_args.size() != 2)
            return (std::cerr << "Syntax error: Invalid error_pages at line: " << line + 1 << "." << std::endl, -1);

        new_server.error_pages[token_args[0]] = token_args[1];
    }
    else if (current_word == "body_size")
    {
        if (token_args.size() != 1)
            return (std::cerr << "Syntax error: Invalid body_size at line: " << line + 1 << "." << std::endl, -1);
        for (size_t j = 0; j < token_args[0].size(); ++j)
        {
            if (!std::isdigit(token_args[0][j]))
                return (std::cerr << "Syntax error: Invalid body_size value at line: " << line + 1 << "." << std::endl,
                        -1);
        }
        std::istringstream iss(token_args[0]);
        iss >> new_server.body_size;
    }
    else
        return (std::cerr << "Syntax error: Invalid keyword at line: " << line + 1 << "." << std::endl, -1);
    return 0;
}

int Server::parsing_config()
{
    bool in_server = false;
    bool in_routes = false;

    server_data new_server;
    routes_data new_routes;
    for (size_t line = 0; line < _file_config_content.size(); ++line)
    {
        for (size_t i = 0; i < _file_config_content[line].size(); ++i)
        {
            std::string current_word = _file_config_content[line][i];
            if (current_word == "server")
            {
                if (in_server)
                    return (std::cerr << "Syntax error: We are already in a server block at line: " << line + 1 << "."
                                      << std::endl,
                            -1);
                in_server = true;
                ++i;
                continue;
            }
            else if (current_word == "routes")
            {
                if (!in_server)
                    return (std::cerr << "Syntax error: We are not in a server block at line: " << line + 1 << "."
                                      << std::endl,
                            -1);
                if (in_routes)
                    return (std::cerr << "Syntax error: We are already in a routes block at line: " << line + 1 << "."
                                      << std::endl,
                            -1);
                in_routes = true;
                ++i;
                continue;
            }
            else if (current_word == "}")
            {
                if (in_routes)
                {
                    new_server.routes.push_back(new_routes);
                    in_routes = false;
                }
                else if (in_server)
                {
                    servers.push_back(new_server);
                    in_server = false;
                }
                continue;
            }
            else if (current_word == "{")
                continue;

            ++i;
            if (_file_config_content[line][i] != "=")
                return (std::cerr << "Syntax error: Missing '=' at line: " << line + 1 << "." << std::endl, -1);
            ++i;
            std::vector<std::string> token_args;
            while (i < _file_config_content[line].size())
            {
                if (_file_config_content[line][i] == ";")
                    break;
                if (_file_config_content[line][i][0] != '"' ||
                    _file_config_content[line][i][_file_config_content[line][i].size() - 1] != '"')
                    return (std::cerr << "Syntax error: Missing '\"' at line: " << line + 1 << "." << std::endl, -1);
                // Remove quotes
                token_args.push_back(_file_config_content[line][i].substr(1, _file_config_content[line][i].size() - 2));
                ++i;
            }
            if (token_args.empty())
                return (std::cerr << "Syntax error: Missing arguments at line: " << line + 1 << "." << std::endl, -1);
            if (i < _file_config_content[line].size() - 1 && _file_config_content[line][i + 1] != ";")
                return (std::cerr << "Syntax error: Missing ';' at line: " << line + 1 << "." << std::endl, -1);

            if (in_routes && token_args.size() >= 1)
            {
                if (parsing_routes(token_args, new_routes, current_word, line) == -1)
                    return -1;
            }
            else if (in_server && token_args.size() >= 1)
            {
                if (parsing_server(token_args, new_server, current_word, line) == -1)
                    return -1;
            }
            else
                return (std::cerr << "Syntax error: Invalid keyword at line: " << line + 1 << "." << std::endl, -1);
        }
    }
    return 0;
}

/* --------------- Socket --------------- */

int Server::open_socket()
{
    _listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listen_fd == -1)
        return -1;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080); // Example port

    if (bind(_listen_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
        return -1;

    if (listen(_listen_fd, SOMAXCONN) < 0)
        return -1;

    _epoll_fd = epoll_create(1);
    if (_epoll_fd == -1)
        return -1;

    epoll_event event;
    event.events = EPOLLIN | EPOLLET; // Edge-triggered read events
    event.data.fd = _listen_fd;

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _listen_fd, &event) < 0)
        return -1;

    return 0;
}

int Server::run()
{
    epoll_event events[MAX_EVENTS];

    while (true)
    {
        int num_events = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
        if (num_events < 0)
            return -1;

        for (int i = 0; i < num_events; ++i)
        {
            if (events[i].data.fd == _listen_fd)
            {
                // Handle new connection
                sockaddr_in client_addr;
                socklen_t client_addr_size = sizeof(client_addr);
                int client_fd = accept(_listen_fd, (sockaddr *)&client_addr, &client_addr_size);

                if (client_fd != -1)
                {
                    fcntl(client_fd, F_SETFL, O_NONBLOCK);
                    epoll_event client_event;
                    client_event.events = EPOLLIN | EPOLLET; // Edge-triggered read events
                    client_event.data.fd = client_fd;
                    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
                }
            }
            else
            {
                std::string request;
                char buffer[4096];
                int bytes_read = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                if (bytes_read > 0)
                {
                    buffer[bytes_read] = '\0';
                    request += buffer;
                }
                else
                {
                    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                }

                Response response(events[i].data.fd, request.c_str());
                response.sendResponse();
            }
        }
    }
    return 0;
}