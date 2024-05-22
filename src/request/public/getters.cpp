#include "Request.hpp"

const Server &Request::get_server() const
{
	return _server;
}

const int &Request::get_client_fd() const
{
	return _client_fd;
}

const std::string &Request::get_request() const
{
	return _request;
}

const std::map<std::string, std::string> &Request::get_headers() const
{
	return _headers;
}

const std::string &Request::get_body() const
{
	return _body;
}
