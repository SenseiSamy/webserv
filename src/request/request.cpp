#include "Request.hpp"
#include "Server.hpp"
#include "Utils.hpp"

/* Functions */
#include <cstddef>
#include <iostream> // std::cout, std::endl
#include <sstream>	// std::istringstream

Request::Request(Server &server, const char *request, const int &client_fd)
		: _server(server), _client_fd(client_fd), _request(request), _state(State::START), _content_length(0)
{
	if (_state == State::ERROR)
		_server.set_status_code(400);
	if (_state == State::COMPLETE)
	{
		std::cout << _server.get_server().host << ":" << _server.get_server().port << " - - \"" << _method << " " << _uri
							<< " " << _version << "\" ";
	}
}

Request::~Request()
{
}

void Request::concatenate_request(const std::string &request)
{
	for (size_t i = 0; i < request.size(); i++)
	{
		_request.push_back(request[i]);

		if (_state == State::START && _request.size() >= 2 && _request.substr(_request.size() - 2) == "\r\n")
		{
			_state = State::HEADERS;
			parse_request_line();
		}
		else if (_state == State::HEADERS && _request.size() >= 4 && _request.substr(_request.size() - 4) == "\r\n\r\n")
		{
			_state = State::BODY;
			parse_headers();
		}
		else if (_state == State::BODY)
		{
			if (!_tmp_body.is_open())
			{
				_tmp_body.open("/tmp/body_" + std::to_string(_client_fd), std::ios::out | std::ios::trunc);
				if (!_tmp_body.is_open())
				{
					_server.set_status_code(500);
					break;
				}
			}

			if (_content_length > 0)
			{
				_content_length--;
				_tmp_body << request[i];
			}
			if (_content_length == 0)
			{
				_state = State::COMPLETE;
				_tmp_body.close();
				break;
			}
		}
	}
}

void Request::parse_request_line()
{
	std::istringstream first_line(_request);
	first_line >> _method >> _uri >> _version;

	if (_method.empty() || _uri.empty() || _version.empty())
		_server.set_status_code(400);

	else if (_method != "GET" && _method != "POST" && _method != "PUT" && _method != "DELETE")
		_server.set_status_code(405);

	else if (_version != "HTTP/1.1")
		_server.set_status_code(505);

	_state = State::HEADERS;
}

void Request::parse_headers()
{
	std::istringstream iss(_request);
	std::string line;
	std::getline(iss, line);
	while (std::getline(iss, line))
	{
		if (line.empty())
			break;
		std::istringstream iss_header(line);
		std::string key, value;
		std::getline(iss_header, key, ':');
		std::getline(iss_header, value);
		_headers[key] = value;
	}

	const std::string &content_length = _headers["Content-Length"];

	if (content_length.empty())
		_server.set_status_code(411);
	else
		_content_length = to_size_t(content_length);
}

bool Request::is_complet()
{
	return _state == State::COMPLETE;
}
