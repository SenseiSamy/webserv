#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#define END_BLOCK '}'
#define START_BLOCK '{'
#define COMMENT '//'
#define END_LINE ';'

bool Config::parseConfig()
{
    std::ifstream configFile(_path_config.c_str());
    if (!configFile.is_open())
        return false;

    std::string line;
    ServerConfig currentServerConfig;
    while (std::getline(configFile, line))
    {
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "server")
        {
            if (!currentServerConfig.listen.empty())
            {
                _servers.push_back(currentServerConfig);
                currentServerConfig = ServerConfig();
            }
            parseServerBlock(configFile, currentServerConfig);
        }
    }

    if (!currentServerConfig.listen.empty())
        _servers.push_back(currentServerConfig);

    return true;
}

const std::vector<Config::ServerConfig> &Config::getServers() const
{
    return _servers;
}

void Config::parseServerBlock(std::istream &stream, ServerConfig &serverConfig)
{
    std::string line;
    while (std::getline(stream, line))
    {
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "listen")
            iss >> serverConfig.listen;
        else if (key == "server_name")
        {
            std::string name;
            while (iss >> name)
                serverConfig.server_name.push_back(name);
        }
        else if (key == "error_page")
            iss >> serverConfig.error_page;
        else if (key == "client_max_body_size")
            iss >> serverConfig.client_max_body_size;
        else if (key == "location")
        {
            std::string path;
            iss >> path;
            LocationConfig locationConfig;
            parseLocationBlock(stream, locationConfig);
            serverConfig.locations[path] = locationConfig;
        }
        else if (key == "}")
            break;
    }
}

void Config::parseLocationBlock(std::istream &stream, LocationConfig &locationConfig)
{
    std::string line;
    while (std::getline(stream, line))
    {
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        // Similar parsing logic for location-specific keys
        if (key == "limit_except")
        {
            std::string method;
            while (iss >> method)
                locationConfig.limit_except.push_back(method);
        }
        else if (key == "return_directive")
            iss >> locationConfig.return_directive;
        else if (key == "root")
            iss >> locationConfig.root;
        else if (key == "autoindex")
        {
            std::string value;
            iss >> value;
            locationConfig.autoindex = (value == "on");
        }
        else if (key == "index")
            iss >> locationConfig.index;
        else if (key == "fastcgi_pass")
            iss >> locationConfig.fastcgi_pass;
        else if (key == "client_max_body_size")
            iss >> locationConfig.client_max_body_size;
        else if (key == "fastcgi_param_SCRIPT_FILENAME")
            iss >> locationConfig.fastcgi_param_SCRIPT_FILENAME;
        else if (key == "}")
            break;
    }
}

void Config::displayConfig() const
{
    for (std::vector<Config::ServerConfig>::const_iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
        const Config::ServerConfig &server = *it;
        std::cout << "Server: " << server.listen << std::endl;

        // Indent server names
        for (std::vector<std::string>::const_iterator nameIt = server.server_name.begin(); nameIt != server.server_name.end(); ++nameIt)
            std::cout << "    Server name: " << *nameIt << std::endl;

        std::cout << "    Error page: " << server.error_page << std::endl;
        std::cout << "    Client max body size: " << server.client_max_body_size << std::endl;

        // Indent locations
        for (std::map<std::string, Config::LocationConfig>::const_iterator locIt = server.locations.begin(); locIt != server.locations.end(); ++locIt)
        {
            const std::string &location = locIt->first;
            const Config::LocationConfig &locationConfig = locIt->second;
            std::cout << "    Location: " << location << std::endl;

            // Further indent location parameters
            std::cout << "        Limit except: ";
            for (std::vector<std::string>::const_iterator methodIt = locationConfig.limit_except.begin(); methodIt != locationConfig.limit_except.end(); ++methodIt)
                std::cout << *methodIt << " ";
            std::cout << std::endl;

            std::cout << "        Return directive: " << locationConfig.return_directive << std::endl;
            std::cout << "        Root: " << locationConfig.root << std::endl;
            std::cout << "        Autoindex: " << (locationConfig.autoindex ? "on" : "off") << std::endl;
            std::cout << "        Index: " << locationConfig.index << std::endl;
            std::cout << "        Fastcgi pass: " << locationConfig.fastcgi_pass << std::endl;
            std::cout << "        Client max body size: " << locationConfig.client_max_body_size << std::endl;
            std::cout << "        Fastcgi param SCRIPT_FILENAME: " << locationConfig.fastcgi_param_SCRIPT_FILENAME << std::endl;
        }
        std::cout << std::endl; // Add a newline for better separation between servers
    }
}
