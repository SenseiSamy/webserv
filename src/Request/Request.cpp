#include "Request.hpp"

#include <sstream>
#include <string>
#include <unistd.h>
#include <iostream>

Request::Request(): _request("")
{
}

Request::Request(const std::string &request): _request(request)
{
	parse();
}

Request::Request(const Request &request)
{
	if (this != &request)
	{
		_request = request._request;
		_method = request._method;
		_url = request._url;
		_headers = request._headers;
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
		_request = request._request;
		_method = request._method;
		_url = request._url;
		_headers = request._headers;
		_content_length = request._content_length;
		_body = request._body;
	}
	return *this;
}

void Request::clear()
{
	_request.clear();
	_method.clear();
	_url.clear();
	_headers.clear();
	_content_length = 0;
	_body.clear();
}

static const std::string trim(const std::string &str)
{
	std::string::size_type first = str.find_first_not_of(' ');
	if (std::string::npos == first)
		return str;
	std::string::size_type last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

void Request::parse()
{
	std::istringstream iss(_request);
	std::string request_line;

	if (!std::getline(iss, request_line))
		return;

	// Extract request line
	std::istringstream request_iss(request_line);
	if (!(request_iss >> _method >> _url))
		return;

	// Extract query string
	std::string::size_type i = _url.find("?");
	if (i != std::string::npos)
	{
		_query_string = _url.substr(i + 1);
		_url = _url.substr(0, i);
	}

	// Extract headers lines
	std::string line;
	while (std::getline(iss, line) && !line.empty())
	{
		std::istringstream header_iss(line);
		std::string header_name;
		std::string header_value;

		if (std::getline(header_iss, header_name, ':') && std::getline(header_iss, header_value)) 
		{
			header_name = trim(header_name);
			header_value = trim(header_value);
			_headers[header_name] = header_value;
		}

		if (_method == "POST" && _headers.find("Content-Length") != _headers.end())
		{
			std::istringstream(_headers["Content-Length"]) >> _content_length;
			_body.assign(std::istreambuf_iterator<char>(iss), std::istreambuf_iterator<char>()); // Read the rest of the stream
		}
	}
}