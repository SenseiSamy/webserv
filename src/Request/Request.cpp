#include "Request.hpp"

#include <sstream>
#include <string>
#include <unistd.h>
#include <iostream>

Request::Request(): _request(""), _header_complete(false)
{
}

Request::Request(const std::string &request): _request(request), _header_complete(false)
{
	if (is_header_complete())
		parse();
}

Request::Request(const Request &request)
{
	if (this != &request)
	{
		_request = request._request;
		_method = request._method;
		_uri = request._uri;
		_version = request._version;
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
		_uri = request._uri;
		_headers = request._headers;
		_content_length = request._content_length;
		_body = request._body;
		_header_complete = request._header_complete;
	}
	return *this;
}

Request &Request::operator+=(const std::string& str)
{
	_request += str;
	if (is_header_complete())
		parse();
	return (*this);
}

void Request::clear()
{
	_request.clear();
	_method.clear();
	_uri.clear();
	_headers.clear();
	_content_length = 0;
	_body.clear();
}

bool Request::is_header_complete()
{
	if (_header_complete)
		return (true);
	if (_request.find("\r\n\r\n") != std::string::npos) {
		_header_complete = true;
		return (true);
	}
	return (false);
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
	if (!(request_iss >> _method >> _uri >> _version))
		return;

	// Extract query string
	std::string::size_type i = _uri.find("?");
	if (i != std::string::npos)
	{
		_query_string = _uri.substr(i + 1);
		_uri = _uri.substr(0, i);
	}

	// Extract headers lines
	std::string line;
	while (std::getline(iss, line) && !line.empty())
	{
		if (line == "\r")
			break;
		std::istringstream header_iss(line);
		std::string header_name;
		std::string header_value;

		if (std::getline(header_iss, header_name, ':') && std::getline(header_iss, header_value)) 
		{
			header_name = trim(header_name);
			header_value = trim(header_value);
			_headers[header_name] = header_value.substr(0, header_value.size() - 1);
		}

		//if (_method == "POST" && _headers.find("Content-Length") != _headers.end())
		//{
		//	std::istringstream(_headers["Content-Length"]) >> _content_length;
		//	_body.assign(std::istreambuf_iterator<char>(iss), std::istreambuf_iterator<char>()); // Read the rest of the stream
		//}	
	}
	if (_method == "POST") {
		int i = 0;
		std::string name;
		while (!tmp_file.is_open()) {
			std::stringstream ss;
			ss << i++;
			name = "/tmp/" + std::string("webserv") + ss.str();
			tmp_file.open(name.c_str(), std::ios::out | std::ios::app | std::ios::binary);
		}
		while (std::getline(iss, line)) {
			tmp_file.write(line.c_str(), line.size() - 1);
		}
	}
}
