#include "Request.hpp"

#include <sstream>

Request::Request() : _client_fd(0), _request(""), _method(""), _uri(""), _header(), _content_length(0), _body("")
{
}

Request::Request(int listen_fd, const std::string &request)
    : _client_fd(listen_fd), _request(request), _method(""), _uri(""), _header(), _content_length(0), _body("")
{
    parse();
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
        _http_version = request._http_version;
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
    _http_version.clear();
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
    display_request();

    // parse the request
    std::istringstream iss(_request);
    std::string line;

    // parse the request line
    std::getline(iss, line);
    std::istringstream iss_line(line);
    iss_line >> _method >> _uri >> _http_version;

    // parse the headers
    while (std::getline(iss, line) && line != "\r")
    {
        std::string key, value;
        std::istringstream iss_line(line);
        iss_line >> key >> value;
        _header[key] = value;
    }

    // parse the body
    std::getline(iss, _body);

    // set the content length
    _content_length = _body.size();
}