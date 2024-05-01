#include "Request.hpp"

#include <sstream>

Request::Request() : _client_fd(0), _request(""), _method(""), _uri(""), _header(), _content_length(0), _body("")
{
}

Request::Request(int listen_fd, const std::string &request)
    : _client_fd(listen_fd), _request(request), _method(""), _uri(""), _header(), _content_length(0), _body("")
{
}

Request::Request(const Request &request)
{
    if (this != &request)
    {
        _client_fd = request._client_fd;
        _request = request._request;
        _method = request._method;
        _uri = request._uri;
        _header = request._header;
        _content_length = request._content_length;
        _body = request._body;
    }
}

Request::~Request()
{
}

Request &Request::operator=(const Request &request)
{
    if (this != &request)
    {
        _client_fd = request._client_fd;
        _request = request._request;
        _method = request._method;
        _uri = request._uri;
        _header = request._header;
        _content_length = request._content_length;
        _body = request._body;
    }
    return *this;
}

void Request::clear()
{
    _client_fd = 0;
    _request.clear();
    _method.clear();
    _uri.clear();
    _header.clear();
    _content_length = 0;
    _body.clear();
}

void Request::parse_request_line()
{
}

void Request::parse()
{
    // clear the request
    clear();

    // create a string stream from the request
    std::stringstream ss(_request);

    // get the method and uri
    ss >> _method >> _uri;

    // get the header
    std::string line;
    while (std::getline(ss, line) && line != "\r")
    {
        size_t pos = line.find(':');
        if (pos != std::string::npos)
            _header[line.substr(0, pos)] = line.substr(pos + 2);
    }

    // get the content-length
    std::string content_length = get_header_key("Content-Length");
    std::stringstream content_length_ss(content_length);
    content_length_ss >> _content_length;

    // get the body
    _body = ss.str();
}