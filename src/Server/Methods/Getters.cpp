#include "Server.hpp"

std::vector<server> Server::get_servers() const
{
    return _servers;
}

std::vector<route> Server::get_route(const size_t i) const
{
    return _servers[i].routes;
}
