#include "Server.hpp"

const int &Server::get_server_fd() const
{
	return _server_fd;
}

const int &Server::get_epoll_fd() const
{
	return _epoll_fd;
}

const server &Server::get_server() const
{
	return _server;
}

const std::map<unsigned short, std::string> &Server::get_error_codes() const
{
	return _error_codes;
}

const unsigned short &Server::get_status_code() const
{
	return _status_code;
}

const std::string &Server::get_host() const
{
	return _host;
}

const std::string &Server::get_methods() const
{
	return _methods;
}

const std::string &Server::get_uri() const
{
	return _uri;
}

const std::string &Server::get_version() const
{
	return _version;
}
