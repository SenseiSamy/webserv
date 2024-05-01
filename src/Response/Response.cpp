#include "Response.hpp"

#include <iostream>
#include <sstream>

Response::Response()
    : _status_code(0), _type(0), _status_message(""), _body(""), _header(), _request(), _error_map(), _uri(""),
      _path_root(""), _server()
{
}

Response::Response(const Request &request, const server &server)
    : _status_code(0), _type(0), _status_message(""), _body(""), _header(), _request(request), _error_map(), _uri(""),
      _path_root(""), _server(server)
{
}

Response::Response(const Response &response)
{
    if (this != &response)
    {
        _status_code = response._status_code;
        _type = response._type;
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
        _type = response._type;
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

int Response::_get()
{
    _status_code = 200;
    _status_message = "OK";
    _header["Content-Type"] = "text/plain";
    _body = "Hello World!";
    return 0;
}

int Response::_post()
{
    return 0;
}

int Response::_delete()
{
    return 0;
}

std::string Response::to_string(size_t i) const
{
    std::ostringstream oss;
    oss << i;
    return oss.str();
}

int Response::send_response()
{
    if (_request.get_method() == "GET")
        _get();
    else if (_request.get_method() == "POST")
        _post();
    else if (_request.get_method() == "DELETE")
        _delete();

    // send response
    std::string response = "HTTP/1.1 " + to_string(_status_code) + " " + _status_message + "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); ++it)
        response += it->first + ": " + it->second + "\r\n";
    response += "\r\n" + _body;

    std::cout << response << std::endl;

    // send response to client
    send(_request.get_client_fd(), response.c_str(), response.size(), 0);

    return 0;
}
