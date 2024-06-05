#include "Request.hpp"
#include "Server.hpp"

void Server::set_verbose(const bool &verbose)
{
	_verbose = verbose;
}

void Server::set_epoll_fd(const int &epoll_fd)
{
	_epoll_fd = epoll_fd;
}

void Server::set_servers(const std::vector<server> &servers)
{
	_servers = servers;
}

void Server::set_requests(const std::map<int, Request> &requests)
{
	_requests = requests;
}

void Server::set_config_file(const std::string &config_file)
{
	_config_file = config_file;
}

void Server::set_content_file(const std::map<size_t, std::vector<std::string> > &content_file)
{
	_content_file = content_file;
}

void Server::set_current_word(const size_t &current_word)
{
	_current_word = current_word;
}

void Server::set_current_line(const size_t &current_line)
{
	_current_line = current_line;
}
