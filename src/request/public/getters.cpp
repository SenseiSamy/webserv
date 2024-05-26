#include "Request.hpp"

const int &Request::get_client_fd() const
{
	return _client_fd;
}

const std::string &Request::get_request() const
{
	return _request;
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
