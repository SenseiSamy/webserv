#include "Server.hpp"

#include <iostream>

void Server::display() const
{
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Servers:" << std::endl;
    for (size_t i = 0; i < _servers.size(); i++)
    {
        std::cout << "  Server " << i << ":" << std::endl;
        std::cout << "    host: " << _servers[i].host << std::endl;
        std::cout << "    port: " << _servers[i].port << std::endl;
        std::cout << "    root: " << _servers[i].root << std::endl;
        std::cout << "    server_names: ";
        for (size_t j = 0; j < _servers[i].server_names.size(); j++)
            std::cout << _servers[i].server_names[j] << " ";
        std::cout << std::endl;
        std::cout << "    default_server: " << _servers[i].default_server << std::endl;
        std::cout << "    max_body_size: " << _servers[i].max_body_size << std::endl;
        std::cout << "    error_pages: ";
        for (std::map<std::string, std::string>::const_iterator it = _servers[i].error_pages.begin();
             it != _servers[i].error_pages.end(); it++)
            std::cout << it->first << " " << it->second << " ";
        std::cout << std::endl;
        std::cout << "    routes:" << std::endl;
        for (size_t j = 0; j < _servers[i].routes.size(); j++)
        {
            std::cout << "      Route " << j << ":" << std::endl;
            std::cout << "        path: " << _servers[i].routes[j].path << std::endl;
            std::cout << "        accepted_methods: ";
            for (size_t k = 0; k < _servers[i].routes[j].accepted_methods.size(); k++)
                std::cout << _servers[i].routes[j].accepted_methods[k] << " ";
            std::cout << std::endl;
            std::cout << "        root: " << _servers[i].routes[j].root << std::endl;
            std::cout << "        redirect: " << _servers[i].routes[j].redirect << std::endl;
            std::cout << "        _directory_listing: " << _servers[i].routes[j]._directory_listing << std::endl;
            std::cout << "        default_file: " << _servers[i].routes[j].default_file << std::endl;
            std::cout << "        cgi: ";
            for (std::map<std::string, std::string>::const_iterator it = _servers[i].routes[j].cgi.begin();
                 it != _servers[i].routes[j].cgi.end(); it++)
                std::cout << it->first << " " << it->second << " ";
            std::cout << "        cgi_upload_path: " << _servers[i].routes[j].cgi_upload_path << std::endl;
            std::cout << "        cgi_upload_enable: " << _servers[i].routes[j].cgi_upload_enable << std::endl;
        }
    }
    std::cout << "----------------------------------------" << std::endl;
}
