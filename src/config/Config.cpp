#include "Config.hpp"

bool Config::parseConfig()
{

    return true;
}

const std::vector<Config::ServerConfig> &Config::getServers() const
{
    return servers;
}

void Config::parseServerBlock(std::istream &stream, ServerConfig &serverConfig)
{
    // Parse a server block from the configuration file.
}

void Config::parseRouteBlock(std::istream &stream, RouteConfig &routeConfig)
{
    // Parse a route block from the configuration file.
}
