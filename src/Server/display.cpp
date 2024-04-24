#include "Server.hpp"

#include <iostream>

void Server::display_servers() const
{
    for (size_t i = 0; i < _servers.size(); i++)
    {
        std::cout << "Server " << i << ":" << std::endl;
        std::cout << "Host: " << _servers[i].host << std::endl;
        std::cout << "Port: " << _servers[i].port << std::endl;
        std::cout << "Server names:" << std::endl;
        for (size_t j = 0; j < _servers[i].server_names.size(); j++)
            std::cout << _servers[i].server_names[j] << std::endl;
        std::cout << "Error pages:" << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = _servers[i].error_pages.begin();
             it != _servers[i].error_pages.end(); ++it)
            std::cout << it->first << ": " << it->second << std::endl;
        std::cout << "Body size: " << _servers[i].body_size << std::endl;
        std::cout << "Routes:" << std::endl;

        for (std::vector<routes_data>::const_iterator it = _servers[i].route.begin(); it != _servers[i].route.end();
             ++it)
        {
            std::cout << "Route:" << it->root << std::endl;
            std::cout << "Methods:" << std::endl;
            for (std::vector<std::string>::const_iterator it2 = it->methods.begin(); it2 != it->methods.end(); ++it2)
                std::cout << *it2 << std::endl;
            std::cout << "Redirect: " << it->redirect << std::endl;
            std::cout << "Autoindex: " << it->autoindex << std::endl;
            std::cout << "Index: " << it->index << std::endl;
            std::cout << "CGI:" << std::endl;
            for (std::map<std::string, std::string>::const_iterator it2 = it->cgi.begin(); it2 != it->cgi.end(); ++it2)
                std::cout << it2->first << ": " << it2->second << std::endl;
            std::cout << "Routes pages:" << std::endl;
            for (std::map<std::string, std::string>::const_iterator it2 = it->routes_pages.begin();
                 it2 != it->routes_pages.end(); ++it2)
                std::cout << it2->first << ": " << it2->second << std::endl;
        }
    }
}

void Server::display_file_config_content() const
{
    for (size_t i = 0; i < _file_config_content.size(); i++)
    {
        for (size_t j = 0; j < _file_config_content[i].size(); j++)
            std::cout << _file_config_content[i][j] << std::endl;
    }
}

void Server::display_path() const
{
    std::cout << "Path: " << _path << std::endl;
}
