#include "Request.hpp"

#include <sstream>

Request::Request(const std::string& request) : _request(request)
{
    parseRequest();
}

Request::Request(const Request& other)
{
    _request = other._request;
    _method = other._method;
    _url = other._url;
    _headers = other._headers;
    _content_lenght = other._content_lenght;
    _body = other._body;
}

Request::~Request()
{
}

Request& Request::operator=(const Request& other)
{
    if (this != &other)
    {
        _request = other._request;
        _method = other._method;
        _url = other._url;
        _headers = other._headers;
        _content_lenght = other._content_lenght;
        _body = other._body;
    }
    return *this;
}

void Request::parseRequest()
{
    std::istringstream stream(_request);
    std::string requestLine;
    _headers[""] = "";
    if (!std::getline(stream, requestLine))
        return;

    // Extract request line
    std::istringstream requestLineStream(requestLine);
    if (!std::getline(requestLineStream, _method, ' ') || !std::getline(requestLineStream, _url, ' '))
        return;

    // Extract headers lines
    std::string line;
    while (std::getline(stream, line))
    {
        std::string headername;
        std::string headervalue;
        size_t colonPos = line.find_first_of(':');
        if (line.empty() || colonPos == std::string::npos)
            break;
        else
        {
            headername = line.substr(0, colonPos);
            headervalue = line.substr(colonPos + 1);
            headervalue.erase(headervalue.size() - 1);
            while (headername.find('\t') != std::string::npos)
                headername = headername.substr(headername.find('\t'));
            _headers[headername] = headervalue;
        }
    }
    if (!_method.compare("POST"))
    {
        std::stringstream temp;
		temp << _headers["Content-Length"];
		temp >> _content_lenght;
        _body += "\r\n\r\n";
		size_t	count = 0;
		char buff[5000];
		while (count < _content_lenght && !stream.eof())
		{
			stream.read(buff, 5000);
			_body.append(buff, stream.gcount());
		}
    }
}
