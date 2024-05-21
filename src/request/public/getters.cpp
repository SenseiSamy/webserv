#include "Request.hpp"

const Server &Request::get_server() const
{
	return _server;
}

const std::string &Request::get_method() const
{
	return _method;
}

const std::string &Request::get_uri() const
{
	return _uri;
}

const std::string &Request::get_version() const
{
	return _version;
}

const std::map<std::string, std::string> &Request::get_headers() const
{
	return _headers;
}

const std::string &Request::get_body() const
{
	return _body;
}

