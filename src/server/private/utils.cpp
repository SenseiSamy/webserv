#include "Utils.hpp"
#include "Server.hpp"

void Server::_request_line(const std::string &request)
{
	const std::string first_line = request.substr(0, request.find("\r\n"));
	const std::vector<std::string> tokens = split(first_line, ' ');

	if (tokens.size() != 3)
	{
		_status_code = 400;
		return;
	}

	_methods = tokens[0];
	_uri = tokens[1];
	_version = tokens[2];

	// check if the HTTP version is valid
	if (_version != "HTTP/1.1")
		_status_code = 505;

	if (_methods != "GET" && _methods != "HEAD" && _methods != "POST" && _methods != "PUT" && _methods != "DELETE" &&
			_methods != "CONNECT" && _methods != "OPTIONS" && _methods != "TRACE")
		_status_code = 501;
}
