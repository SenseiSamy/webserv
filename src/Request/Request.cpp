#include "Request.hpp"
#include "Server.hpp"

#include <sstream>
#include <string>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <fcntl.h>

Request::Request(): _request(""), _state(incomplete)
{
}

Request::Request(const std::string &request): _request(request), _state(incomplete)
{
	refresh_state();
	if (_state == header_complete) {
		parse();
		refresh_state();
	}
}

Request::~Request()
{}

Request::Request(const Request &other): _request(other._request), _method(other._method), _uri(other._uri), _version(other._version), _headers(other._headers), _content_length(other._content_length), _body(other._body), _query_string(other._query_string), _state(other._state), _file_size(other._file_size), _file_name(other._file_name)
{}

Request &Request::operator+=(const std::string& str)
{
	switch (_state)
	{
		case incomplete:
			_request += str;
			refresh_state();
			if (_state == header_complete)
				parse();
			break;
		case header_complete:
			if (str.size() + _file_size > _content_length) {
				_tmp_file.write(str.c_str(), _content_length - _file_size);
				_file_size += _content_length - _file_size;
			}
			else {
				_tmp_file.write(str.c_str(), str.size());
				_file_size += str.size();
			}
			break;	
		default:
			std::cerr << "on est pas trop cense arriver la imo";
	}
	refresh_state();
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
	if (!_file_name.empty() && access(_file_name.c_str(), F_OK) == 0)
		std::remove(_file_name.c_str());
}

void Request::refresh_state()
{
	if (_state == incomplete) {
		if (_request.find("\r\n\r\n") != std::string::npos)
			_state = header_complete;
	}
	else if (_state == header_complete) {
		if (_method != "POST")
			_state = complete;
		else {
			if (_content_length == 0)
				_state = invalid;
			else if (_file_size == _content_length)
			{
				_state = complete;
				_tmp_file.close();
			}
		}
	}
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
	if (_headers.find("Content-Length") != _headers.end())
		std::istringstream(_headers["Content-Length"]) >> _content_length;
	else
		_content_length = 0;

	_server = Server::find_server(_headers["Host"]);
	if (_method == "POST") {
		_file_size = 0;
		int i = 0;
		do {
			std::stringstream ss;
			ss << i++;
			_file_name = "/tmp/" + std::string("webserv") + ss.str();
			if (access(_file_name.c_str(), F_OK) == 0)
				continue;
			_tmp_file.open(_file_name.c_str(), std::ios::out | std::ios::app | std::ios::binary);
		} while (!_tmp_file.is_open());

		char buffer[MAX_BUFFER_SIZE]; 
		iss.read(buffer, MAX_BUFFER_SIZE);
		_tmp_file.write(buffer, iss.gcount());
		_file_size += iss.gcount();
	}
}
