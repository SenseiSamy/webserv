#include "Request.hpp"
#include "Server.hpp"
#include "Utils.hpp"

/* Functions */
#include <iostream> // std::cout, std::endl
#include <sstream>	// std::istringstream

Request::Request(Server &server, const int &client_fd)
		: _server(server), _client_fd(client_fd), _state(START), _content_received(0), _content_length(0)
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
	std::string::size_type i_end = 0;

	if (_state == START)
	{
		i_end = _stash.find("\r\n");
		if (i_end != std::string::npos)
		{
			parse_request_line(i_end);
			_state = HEADERS;
		}
	}
	if (_state == HEADERS)
	{
		i_end = _stash.find("\r\n\r\n");
		if (i_end != std::string::npos)
		{
			parse_headers(i_end);
			_state = COMPLETE;
		}
	}
	if (_state == BODY)
	{
		if (_content_length == 0)
			_state = COMPLETE;
		else
		{
			if (_content_received >= _content_length)
				_state = COMPLETE;
			else
			{
				_content_received += _stash.size();
				_tmp_body << _stash;
			}
		}
	}
}

void Request::parse_request_line(const std::string::size_type &end)
{
	std::istringstream iss(_stash.substr(0, end));
	std::string token;

	/* Method */
	std::getline(iss, token, ' ');
	set_method(token);

	/* URI */
	std::getline(iss, token, ' ');
	set_uri(token);

	/* Version */
	std::getline(iss, token, ' ');
	set_version(token);

	_state = HEADERS;
	_stash.erase(0, end + 2);
}

void Request::parse_headers(const std::string::size_type &end)
{
	std::istringstream iss(_stash.substr(0, end));
	std::string line;

	while (std::getline(iss, line, '\n'))
	{
		if (line == "\r")
			break;
		std::string::size_type i_colon = line.find(": ");
		if (i_colon != std::string::npos)
		{
			std::string key = line.substr(0, i_colon);
			std::string value = line.substr(i_colon + 2, line.size() - i_colon - 3);
			if (key.empty() || value.empty())
				_server.set_status_code(400);
			else
				_headers[key] = value;
		}
	}
	_stash.erase(0, end + 4);

	std::map<std::string, std::string>::iterator it = _headers.find("Content-Length");
	if (it != _headers.end() && !it->second.empty())
	{
		std::istringstream(it->second) >> _content_length;
		_tmp_body.open("tmp_body");
		if (!_tmp_body.is_open())
			_server.set_status_code(500);
		_state = BODY;
	}
	else
		_server.set_status_code(411);
}

bool Request::is_complet() const
{
	return _state == COMPLETE;
}
