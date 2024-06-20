#include "Response.hpp"

#include <ctime>

std::string get_current_date()
{
    std::time_t now = std::time(NULL);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
    return std::string(buf);
}


Response::Response()
		: _status_code(0), _status_message(""), _body(""), _headers(), _error_codes(), _uri(""), _path_to_root(""),
			_server(), _is_cgi(false)
{
}

Response::Response(const unsigned short error, const server &server,
									 const std::map<unsigned short, std::string> &error_codes)
		: _status_code(error), _status_message(error_codes.at(error)), _body(""), _error_codes(error_codes), _uri(""),
			_path_to_root(""), _server(server), _is_cgi(false)
{
	_headers["Date"] = get_current_date();
	_generate_response_code(error);
}

Response::Response(const Request &request, const server &server,
									 const std::map<unsigned short, std::string> &error_codes)
		: _status_code(0), _status_message(""), _body(""), _headers(), _request(request), _error_codes(error_codes),
			_uri(""), _path_to_root(""), _server(server), _is_cgi(false)
{
	_headers["Date"] = get_current_date();
	_set_root();
	_generate();
}

Response::Response(const Response &other)
{
	if (this != &other)
	{
		_status_code = other._status_code;
		_status_message = other._status_message;
		_body = other._body;
		_headers = other._headers;
		_error_codes = other._error_codes;
		_uri = other._uri;
		_path_to_root = other._path_to_root;
		_is_cgi = other._is_cgi;
		_server = other._server;
	}
}

Response &Response::operator=(const Response &response)
{
	if (this != &response)
	{
		_status_code = response._status_code;
		_status_message = response._status_message;
		_body = response._body;
		_headers = response._headers;
		_error_codes = response._error_codes;
		_uri = response._uri;
		_path_to_root = response._path_to_root;
		_is_cgi = response._is_cgi;
		_server = response._server;
	}
	return *this;
}

Response::~Response()
{
}

void Response::_generate()
{
	_route = NULL;
	const std::string request = _request.get_uri();

	for (size_t i = 0; i < _server.routes.size(); ++i)
	{
		if (request.find(_server.routes[i].path) == 0 )
		{
			if (request.size() == _server.routes[i].path.size() || request[_server.routes[i].path.size()] == '/')
			{
				for (size_t j = 0; j < _server.routes[i].accepted_methods.size(); ++j)
				{
					if (_request.get_method() == _server.routes[i].accepted_methods[j])
					{
						_route = &_server.routes[i];
						break;
					}
				}
				if (_route == NULL)
				{
					_generate_response_code(405); // Method Not Allowed
					return;
				}
			}
		}
		if (_route != NULL)
			break;
	}

	if (_request.get_version() != "HTTP/1.1")
	{
		_generate_response_code(505);
		return;
	}

	const std::string method = _request.get_method();

	if (method == "GET")
		_get();
	else if (method == "POST")
		_post();
	else if (method == "DELETE")
		_delete();
	else
		_generate_response_code(405); // Method Not Allowed
}
