#include "Server.hpp"
#include <cstring>
#include <exception>
#include <netinet/in.h>

const bool &Server::get_verbose() const
{
	return _verbose;
}

const int &Server::get_epoll_fd() const
{
	return _epoll_fd;
}

const std::map<unsigned short, std::string> &Server::get_error_codes() const
{
	return _error_codes;
}

const std::vector<server> &Server::get_servers() const
{
	return _servers;
}

const std::map<int, Request> &Server::get_requests() const
{
	return _requests;
}

const std::string &Server::get_config_file() const
{
	return _config_file;
}

const std::map<size_t, std::vector<std::string> > &Server::get_content_file() const
{
	return _content_file;
}

const size_t &Server::get_current_word() const
{
	return _current_word;
}

const size_t &Server::get_current_line() const
{
	return _current_line;
}

sockaddr_in Server::get_client_addr(int fd) const
{
	try {
		return _client_adresses.at(fd);
	}
	catch (std::exception& e) {
		sockaddr_in a;
		memset(&a, 0, sizeof(a));
		return (a);
	}
}