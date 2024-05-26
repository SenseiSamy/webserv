#include "Request.hpp"
#include "Server.hpp"
#include "Utils.hpp"

/* Functions */
#include <iostream> // std::cout, std::endl
#include <sstream>	// std::istringstream

Request::Request(Server &server, const int &client_fd)
		: _server(server), _client_fd(client_fd), _state(START), _content_length(0), _content_received(0)
{
}

Request::~Request()
{
	if (_tmp_body.is_open())
		_tmp_body.close();
}

std::ostream &operator<<(std::ostream &os, const Request &request)
{
	const Server &server = request.get_server();
	os << server.get_server().host << ":" << server.get_server().port << " - - \"" << request.get_method() << " "
		 << request.get_uri() << " " << request.get_version() << "\"";
	return os;
}

void Request::concatenate_request(const std::string &request)
{
	_stash += request;
	if (_state == START)
	{
		std::string::size_type end = _stash.find("\r\n");
		if (end != std::string::npos)
		{
			parse_request_line(end);
			_state = HEADERS;
		}
	}
	if (_state == HEADERS)
	{
		std::string::size_type end = _stash.find("\r\n\r\n");
		if (end != std::string::npos)
		{
			parse_headers(end);
			_state = COMPLETE;
		}
	}
	if (_state == BODY)
	{
		_content_received += _stash.size();
		if (_content_received >= _content_length)
		{
			_tmp_body << _stash.substr(0, _content_length - (_content_received - _stash.size()));
			_tmp_body.close();
			_state = COMPLETE;
		}
		else
			_tmp_body << _stash;
		_stash.clear();
	}
}

void Request::parse_request_line(const std::string::size_type &end)
{
	std::istringstream iss(_stash.substr(0, end));
	iss >> _method >> _uri >> _version;
	_stash.erase(0, end + 2);

	if (_method != "GET" && _method != "POST" && _method != "PUT" && _method != "DELETE")
		_server.set_status_code(501);
	else if (_version != "HTTP/1.1")
		_server.set_status_code(505);
}

void Request::parse_headers(const std::string::size_type &end)
{
	std::istringstream iss(_stash.substr(0, end));
	std::string line;

	while (std::getline(iss, line, '\n'))
	{
		std::string::size_type pos = line.find(": ");

		if (pos == std::string::npos)
		{
			_server.set_status_code(400);
			return;
		}
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 2, line.size() - pos - 3);
		_headers[key] = value;
		if (key == "Content-Length")
			_content_length = to_size_t(value);
	}
	_stash.erase(0, end + 4);

	if (_headers.find("Content-Length") == _headers.end())
		_server.set_status_code(411);
	else if (_content_length == 0)
		_state = COMPLETE;
	else
	{
		_tmp_body.open("/tmp/body_" + to_string(_client_fd), std::ios::out | std::ios::binary);
		_state = BODY;
	}
}

bool Request::is_complet() const
{
	return _state == COMPLETE;
}
