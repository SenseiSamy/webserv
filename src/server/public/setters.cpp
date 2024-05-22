#include "Server.hpp"

void Server::set_status_code(const unsigned short &status_code)
{
	_status_code = status_code;
}

void Server::set_host(const std::string &host)
{
	_host = host;
}

void Server::set_methods(const std::string &methods)
{
	_methods = methods;
}

void Server::set_uri(const std::string &uri)
{
	_uri = uri;
}

void Server::set_version(const std::string &version)
{
	_version = version;
}
