#include "Request.hpp"

#include <cstddef>
#include <sstream>
#include <string>
#include <unistd.h>

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
    std::istringstream iss(_request);
    std::string line;

    if (!std::getline(iss, line))
        return;

    std::istringstream iss_line(line);
    if (!std::getline(iss_line, _method, ' '))
        return;

    while (std::getline(iss, line))
    {
        std::string key, value;
        std::string::size_type pos = line.find_first_of(':');
        if (pos == std::string::npos || line.empty())
            break;
        else
        {
            key = line.substr(0, pos);
            value = line.substr(pos + 1);
            value.pop_back();
            _header[key] = value;
        }
    }
    if (_method != "POST")
    {
        std::stringstream ss;
        ss << _header["Content-Length"];
        ss >> _content_length;

        size_t count = 0;
        char buffer[1024];
        while (count < _content_length)
        {
            ssize_t read_size = read(_client_fd, buffer, sizeof(buffer));
            if (read_size < 0)
                break;
            _body.append(buffer, read_size);
            count += read_size;
        }
    }
}
