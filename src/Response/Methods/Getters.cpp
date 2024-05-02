#include "Response.hpp"

int Response::get_status_code() const
{
    return _status_code;
}

int Response::get_type() const
{
    return _type;
}

std::string Response::get_status_message() const
{
    return _status_message;
}

std::string Response::get_body() const
{
    return _body;
}

std::map<std::string, std::string> Response::get_header() const
{
    return _header;
}

Request Response::get_request() const
{
    return _request;
}

std::map<unsigned short, std::string> Response::get_error_map() const
{
    return _error_map;
}

std::string Response::get_uri() const
{
    return _uri;
}

std::string Response::get_path_root() const
{
    return _path_root;
}

server Response::get_server() const
{
    return _server;
}
