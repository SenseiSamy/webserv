#include "Request.hpp"

void Request::set_request(const std::string &request)
{
    _request = request;
}

void Request::set_method(const std::string &method)
{
    _method = method;
}

void Request::set_uri(const std::string &uri)
{
    _uri = uri;
}

void Request::set_header(const std::map<std::string, std::string> &header)
{
    _header = header;
}

void Request::set_header_key(const std::string &key, const std::string &value)
{
    _header[key] = value;
}

void Request::set_content_length(const size_t content_length)
{
    _content_length = content_length;
}

void Request::set_body(const std::string &body)
{
    _body = body;
}
