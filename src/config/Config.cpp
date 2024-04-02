#include "Config.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>

Config::Config(const std::string &filePath) : filePath(filePath)
{
}

const std::vector<std::string> Config::split(const std::string &s, const std::string &delim) const
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = s.find_first_of(delim, prev);
        if (pos == std::string::npos)
            pos = s.length();
        if (pos != prev)
            tokens.push_back(s.substr(prev, pos - prev));
        prev = pos + 1;
    } while (pos < s.length() && prev < s.length());
    return tokens;
}

void Config::parseRouteDirective(Route &route, std::string &line, std::ifstream &file)
{
    while (getline(file, line))
    {
        if (line.empty() || line.find_first_not_of(WHITESPACE) == line.find(COMMENT))
            continue;

        std::vector<std::string> tokens = split(line, EQUAL);

        std::string directive = tokens[0];
        if (tokens.size() < 2)
        {
            std::vector<std::string> directiveTokens = split(directive, WHITESPACE);
            if (directiveTokens[0] == "}")
                break;
        }
        std::string value = tokens[1];

        size_t commentPos = value.find(COMMENT);
        if (commentPos != std::string::npos)
            value = value.substr(0, commentPos);

        if ("path" == directive)
            route.path = value;
        else if ("allow_methods" == directive)
        {
            std::vector<std::string> methods = split(value, DELIMITER);
            for (size_t i = 0; i < methods.size(); i++)
            {
                if ("GET" == methods[i])
                    route.allow_methods.push_back(GET);
                else if ("POST" == methods[i])
                    route.allow_methods.push_back(POST);
                else if ("DELETE" == methods[i])
                    route.allow_methods.push_back(DELETE);
                else
                    throw std::runtime_error("Invalid HTTP method: " + methods[i]);
            }
        }
        else if ("redirect_to" == directive)
            route.redirect_to = value;
        else if ("root" == directive)
            route.root = value;
        else if ("directory_listing" == directive)
            route.directory_listing = (value == "on");
        else if ("default_file" == directive)
            route.default_file = value;
        else if ("cgi" == directive)
        {
            std::vector<std::string> cgiTokens = split(value, DELIMITER);
            if (cgiTokens.size() < 2)
            {
                throw std::runtime_error("Invalid CGI directive: " + value);
            }

            route.cgi[cgiTokens[0]] = cgiTokens[1];
        }
        else if ("upload_path" == directive)
            route.upload_path = value;
    }
}

void Config::parseServerDirective(Server &server, std::string &line, std::ifstream &file)
{
    while (getline(file, line))
    {
        if (line.empty() || line.find_first_not_of(WHITESPACE) == line.find(COMMENT))
            continue;

        std::vector<std::string> tokens = split(line, EQUAL);

        std::string directive =
            tokens[0].substr(tokens[0].find_first_not_of(WHITESPACE), tokens[0].find_last_not_of(WHITESPACE) + 1);

        if (tokens.size() < 2)
        {
            std::vector<std::string> directiveTokens = split(directive, WHITESPACE);
            if (directiveTokens[0] == "}")
                break;
            if (directiveTokens[0] == "route")
            {
                Route route;
                parseRouteDirective(route, line, file);
                server.routes.push_back(route);
                continue;
            }
        }
        std::string value = tokens[1];

        size_t commentPos = value.find(COMMENT);
        if (commentPos != std::string::npos)
            value = value.substr(0, commentPos);

        if ("host" == directive)
            server.server_names = split(value, DELIMITER + std::string(WHITESPACE));
        else if ("port" == directive)
            server.port = static_cast<unsigned short>(std::atoi(value.c_str()));
        else if ("error_page" == directive)
        {
            std::vector<std::string> errorTokens = split(value, DELIMITER);

            for (size_t i = 0; i < errorTokens.size(); i++)
            {
                std::vector<std::string> errorPageTokens = split(errorTokens[i], WHITESPACE);

                std::vector<unsigned short> codes;
                for (size_t j = 0; j < errorPageTokens.size() - 1; j++)
                {
                    codes.push_back(static_cast<unsigned short>(std::atoi(errorPageTokens[j].c_str())));
                }

                server.error_pages[codes] = errorPageTokens.back();
            }
        }
        else if ("client_max_body_size" == directive)
            server.client_max_body_size = std::strtoul(value.c_str(), NULL, 10);
        else if ("route" == directive)
        {
            Route route;
            parseRouteDirective(route, line, file);
            server.routes.push_back(route);
        }
    }
}

void Config::parseConfigFile()
{
    std::ifstream file(filePath.c_str());
    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open config file: " + filePath);
    }

    Server *currentServer = NULL;
	(void)currentServer;
    std::string line;
    while (getline(file, line))
    {
        if (line.empty() || line.find_first_not_of(WHITESPACE) == line.find(COMMENT))
            continue;

        std::istringstream iss(line);
        std::string word;
        iss >> word;

        if (word == "server")
        {
            Server server;
            parseServerDirective(server, line, file);
            servers.push_back(server);
            currentServer = &servers.back();
        }
    }

    file.close();
}

void Config::displayConfig() const
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        std::cout << "Server " << i + 1 << std::endl;
        std::cout << "Host: ";
        for (size_t j = 0; j < servers[i].host.size(); j++)
        {
            std::cout << servers[i].host[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Port: " << servers[i].port << std::endl;
        std::cout << "Error Pages: " << std::endl;
        for (std::map<std::vector<unsigned short>, std::string>::const_iterator it = servers[i].error_pages.begin();
             it != servers[i].error_pages.end(); ++it)
        {
            std::cout << "Codes: ";
            for (size_t j = 0; j < it->first.size(); j++)
            {
                std::cout << it->first[j] << " ";
            }
            std::cout << "Path: " << it->second << std::endl;
        }
        std::cout << "Client Max Body Size: " << servers[i].client_max_body_size << std::endl;
        std::cout << "Routes: " << std::endl;
        for (size_t j = 0; j < servers[i].routes.size(); j++)
        {
            std::cout << "Path: " << servers[i].routes[j].path << std::endl;
            std::cout << "Allow Methods: ";
            for (size_t k = 0; k < servers[i].routes[j].allow_methods.size(); k++)
            {
                if (servers[i].routes[j].allow_methods[k] == GET)
                    std::cout << "GET ";
                else if (servers[i].routes[j].allow_methods[k] == POST)
                    std::cout << "POST ";
                else if (servers[i].routes[j].allow_methods[k] == DELETE)
                    std::cout << "DELETE ";
            }
            std::cout << std::endl;
            std::cout << "Redirect To: " << servers[i].routes[j].redirect_to << std::endl;
            std::cout << "Root: " << servers[i].routes[j].root << std::endl;
            std::cout << "Directory Listing: " << servers[i].routes[j].directory_listing << std::endl;
            std::cout << "Default File: " << servers[i].routes[j].default_file << std::endl;
            std::cout << "CGI: " << std::endl;
            for (std::map<std::string, std::string>::const_iterator it = servers[i].routes[j].cgi.begin();
                 it != servers[i].routes[j].cgi.end(); ++it)
            {
                std::cout << "Key: " << it->first << " Value: " << it->second << std::endl;
            }
            std::cout << "Upload Path: " << servers[i].routes[j].upload_path << std::endl;
            for (size_t k = 0; k < servers[i].routes[j].allow_methods.size(); k++)
            {
                if (servers[i].routes[j].allow_methods[k] == GET)
                    std::cout << "GET ";
                else if (servers[i].routes[j].allow_methods[k] == POST)
                    std::cout << "POST ";
                else if (servers[i].routes[j].allow_methods[k] == DELETE)
                    std::cout << "DELETE ";
            }
            std::cout << std::endl;
        }
    }
}
