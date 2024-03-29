#include "Config.hpp"
#include <cstddef>
#include <stdexcept>

void Config::parseConfig()
{
    // try to open the configuration file
    std::ifstream file(config_file.c_str());
    if (!file.is_open())
        throw std::runtime_error("Failed to open configuration file");

    // parse the configuration file
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() \
            || line.find_first_not_of(WHITE_SPACE) == line.find(COMMENT))
            continue;

        // parse the line
        if (line.find("server") != std::string::npos)
        {
            Server server;
            parseServer(file, server);
            servers.push_back(server);
        }
    }
}

int Config::stringToInt(const std::string &str)
{
    int result = 0;
    for (std::size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] < '0' || str[i] > '9')
            throw std::runtime_error("Invalid number");
        result = result * 10 + (str[i] - '0');
    }
    return result;
}

std::vector<std::string> splitLine(const std::string &line)
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    while (start < line.size())
    {
        std::string::size_type end = line.find_first_of(std::string(WHITE_SPACE) + END_LINE, start);
        tokens.push_back(line.substr(start, end - start));
        start = line.find_first_not_of(WHITE_SPACE, end);
    }
    return tokens;
}

void Config::parseServer(std::ifstream &file, Server &server)
{
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() \
            || line.find_first_not_of(WHITE_SPACE) == line.find(COMMENT))
            continue;

        std::vector<std::string> tokens = splitLine(line);
        if (tokens.size() < 2)
            throw std::runtime_error("Invalid server directive");

        if (tokens[0] == "listen")
        {
            // split the host and port
            std::string::size_type colon = tokens[1].find(':');
            if (colon == std::string::npos)
                throw std::runtime_error("Invalid listen directive on line: " + line);
            server.listen = std::make_pair(tokens[1].substr(0, colon), stringToInt(tokens[1].substr(colon + 1)));
        }
        else if (tokens[0] == "server_name")
        {
            for (std::size_t i = 1; i < tokens.size(); ++i)
                server.server_names.push_back(tokens[i]);
        }
        else if (tokens[0] == "error_page")
        {
            if (tokens.size() < 3)
                throw std::runtime_error("Invalid error_page directive on line: " + line);
            for (std::size_t i = 1; i < tokens.size() - 1; ++i)
                server.error_pages[stringToInt(tokens[i])] = tokens.back();

        }
        else if (tokens[0] == "client_max_body_size")
        {
            server.client_max_body_size = stringToInt(tokens[1]);
        }
        else if (tokens[0] == "location")
        {
            if (tokens.size() < 2 || tokens[1] != START_BLOCK)
                throw std::runtime_error("Invalid location directive on line: " + line);
            Location location;
            parseLocation(file, location);
            server.locations[tokens[1]] = location;
        }
    }
}

void Config::parseLocation(std::ifstream &file, Location &location)
{
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() \
            || line.find_first_not_of(WHITE_SPACE) == line.find(COMMENT))
            continue;

        // parse the line
        if (line.find(END_BLOCK) != std::string::npos)
            break;

        std::vector<std::string> tokens = splitLine(line);
        if (tokens.size() < 2)
            throw std::runtime_error("Invalid location directive");

        if (tokens[0] == "limit_except")
        {
            for (std::string::size_type i = 1; i < tokens.size(); ++i)
                location.limit_except.push_back(tokens[i]);
        }
        else if (tokens[0] == "rewrite")
        {
            if (tokens.size() < 3)
                throw std::runtime_error("Invalid rewrite directive on line: " + line);
            location.rewrite = std::make_pair(tokens[1], tokens[2]);
        }
        else if (tokens[0] == "root")
        {
            location.root = tokens[1];
        }
        else if (tokens[0] == "autoindex")
        {
            location.autoindex = tokens[1] == "on";
        }
        else if (tokens[0] == "index")
        {
            location.index = tokens[1];
        }
        else if (tokens[0] == "cgi")
        {
            if (tokens.size() < 3)
                throw std::runtime_error("Invalid CGI directive on line: " + line);
            location.cgi[tokens[1]] = tokens[2];
        }
    }
}
