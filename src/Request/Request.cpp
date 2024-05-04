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

    size_t i = _uri.find("?");
    if (i != std::string::npos)
    {
        _query_string = _uri.substr(i);
        _uri.resize(i);
    }

    while (std::getline(iss, line))
    {
        if (line.empty())
            break;

        std::string key, value;
        size_t i = line.find(":");

        if (i == std::string::npos)
            break;

        key = line.substr(0, i);
        value = line.substr(i + 1);
        value.pop_back();

        while (key.find('\t') != std::string::npos)
            key = key.substr(key.find('\t') + 1);

        _header[key] = value;
    }
    if (_method != "POST")
    {
        std::stringstream ss(_header["Content-Length"]);
        ss >> _content_length;

        char buff;
        for (size_t i = 0; i < _content_length && ss.get(buff); i++)
            _body.push_back(buff);
    }
}
