#include "Request.hpp"

std::string Request::get_method() const
{
    return _method;
}

std::string Request::get_uri() const
{
    return _uri;
}

std::string Request::get_header(const std::string &key)
{
    return _header[key];
}

std::map<std::string, std::string> Request::get_header() const
{
    return _header;
}

size_t Request::get_content_length() const
{
    return _content_length;
}

std::string Request::get_body() const
{
    return _body;
}

std::string Request::get_http_version() const
{
    return _http_version;
}

std::string Request::get_query_string() const
{
    return _query_string;
}

