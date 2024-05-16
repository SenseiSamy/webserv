#include "Response.hpp"

Response::Response(): _status_code(0), _status_message(""), _body(""), _headers(), _request(), _error_codes(), _uri(""), _path_to_root(""), _server(), _is_cgi(false)
{
	_set_root();
	_select_route();
	_generate();
}

Response::Response(const unsigned short error, const server &server, const std::map<unsigned int, std::string> &error_codes): _status_code(error), _status_message(error_codes.at(error)), _body(""), _headers(), _request(), _error_codes(error_codes), _uri(""), _path_to_root(""), _server(server), _is_cgi(false)
{
	_generate_error(error);
}

Response::Response(const Request &request, const server &server, const std::map<unsigned int, std::string> &error_codes): _status_code(0), _status_message(""), _body(""), _headers(), _request(request), _error_codes(error_codes), _uri(""), _path_to_root(""), _server(server), _is_cgi(false)
{
	_set_root();
	_select_route();
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
		_request = other._request;
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
		_request = response._request;
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
	if (_request.get_version() != "HTTP/1.1")
	{
		_generate_error(505);
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
		_generate_error(405);
}
