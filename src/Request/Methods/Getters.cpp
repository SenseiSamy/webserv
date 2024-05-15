#include "Request.hpp"

std::string Request::get_request() const
{
	return _request;
}

std::string Request::get_method() const
{
	return _method;
}

std::string Request::get_uri() const
{
	return _uri;
}

std::string Request::get_version() const
{
	return _version;
}

std::string Request::get_headers_key(const std::string &key)
{
	return _headers[key];
}

std::map<std::string, std::string> Request::get_headers() const
{
	return _headers;
}

size_t Request::get_content_length() const
{
	return _content_length;
}

std::string Request::get_body() const
{
	return _body;
}

std::string Request::get_query_string() const
{
	return _query_string;
}

std::string Request::get_first_line() const
{
	std::string::size_type pos = _request.find("\r\n");
	if (pos == std::string::npos)
		return "";
	return _request.substr(0, pos);
}