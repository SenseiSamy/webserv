#include "Request.hpp"

int Request::get_client_fd() const
{
    return _client_fd;
}

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

std::map<std::string, std::string> Request::get_header() const
{
    return _header;
}

std::string Request::get_header_key(const std::string &key) const
{
    return _header.at(key);
}

size_t Request::get_content_length() const
{
    return _content_length;
}

std::string Request::get_body() const
{
    return _body;
}
