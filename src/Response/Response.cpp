#include "Response.hpp"

#include <iostream>
#include <sstream>

Response::Response()
    : _status_code(0), _status_message(""), _body(""), _header(), _request(), _error_map(), _uri(""),
      _path_root(""), _server()
{
}

Response::Response(const Request &request, const server &server)
    : _status_code(0), _status_message(""), _body(""), _header(), _request(request), _error_map(), _uri(""),
      _path_root(""), _server(server)
{
    
}

Response::Response(const Response &response)
{
    if (this != &response)
    {
        _status_code = response._status_code;
        _status_message = response._status_message;
        _body = response._body;
        _header = response._header;
        _request = response._request;
        _error_map = response._error_map;
        _uri = response._uri;
        _path_root = response._path_root;
        _server = response._server;
    }
}

Response &Response::operator=(const Response &response)
{
    if (this != &response)
    {
        _status_code = response._status_code;
        _status_message = response._status_message;
        _body = response._body;
        _header = response._header;
        _request = response._request;
        _error_map = response._error_map;
        _uri = response._uri;
        _path_root = response._path_root;
        _server = response._server;
    }
    return *this;
}

Response::~Response()
{
}

std::string Response::to_string(size_t i) const
{
    std::ostringstream oss;
    oss << i;
    return oss.str();
}

void Response::send_response()
{
    const std::string method = _request.get_method(); 

    if (method == "GET")
        _get();
    else if (method == "POST")
        _post();
    else if (method == "DELETE")
        _delete();
    else
        _status_code = 405;
}

