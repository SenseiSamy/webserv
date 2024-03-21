#include "Config.hpp"
#include "logger.hpp"
#include <fstream>

bool Config::parseConfig()
{
    // parse line by line the configuration file and if we get a server block, parse it
    std::ifstream file(path_config);
    if (!file.is_open())
    {
        log(ERROR, "Failed to open configuration file: " + path_config);
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.find("server") != std::string::npos)
        {
            ServerConfig serverConfig;
            parseServerBlock(file, serverConfig);
            servers.push_back(serverConfig);
        }
    }
    return true;
}

const std::vector<Config::ServerConfig> &Config::getServers() const
{
    return servers;
}

void Config::parseServerBlock(std::istream &stream, ServerConfig &serverConfig)
{

}

void Config::parseRouteBlock(std::istream &stream, RouteConfig &routeConfig)
{
    // Parse a route block from the configuration file.
}
