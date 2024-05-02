#include "Response.hpp"

void Response::set_status_code(int status_code)
{
    _status_code = status_code;
}

void Response::set_type(int type)
{
    _type = type;
}

void Response::set_status_message(const std::string &status_message)
{
    _status_message = status_message;
}

void Response::set_body(const std::string &body)
{
    _body = body;
}

void Response::set_header(const std::map<std::string, std::string> &header)
{
    _header = header;
}

void Response::set_request(const Request &request)
{
    _request = request;
}

void Response::set_error_map(const std::map<unsigned short, std::string> &error_map)
{
    _error_map = error_map;
}

void Response::set_uri(const std::string &uri)
{
    _uri = uri;
}

void Response::set_path_root(const std::string &path_root)
{
    _path_root = path_root;
}

void Response::set_server(const server &server)
{
    _server = server;
}
