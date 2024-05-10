#include "Response.hpp"

void Response::_error_map_init()
{
	_error_map[100] = "Continue";
	_error_map[101] = "Switching Protocols";
	_error_map[102] = "Processing";
	_error_map[200] = "OK";
	_error_map[201] = "Created";
	_error_map[202] = "Accepted";
	_error_map[203] = "Non-Authoritative Information";
	_error_map[204] = "No Content";
	_error_map[205] = "Reset Content";
	_error_map[206] = "Partial Content";
	_error_map[207] = "Multi-Status";
	_error_map[208] = "Already Reported";
	_error_map[226] = "IM Used";
	_error_map[300] = "Multiple Choices";
	_error_map[301] = "Moved Permanently";
	_error_map[302] = "Found";
	_error_map[303] = "See Other";
	_error_map[304] = "Not Modified";
	_error_map[305] = "Use Proxy";
	_error_map[306] = "(Unused)";
	_error_map[307] = "Temporary Redirect";
	_error_map[308] = "Permanent Redirect";
	_error_map[400] = "Bad Request";
	_error_map[401] = "Unauthorized";
	_error_map[402] = "Payment Required";
	_error_map[403] = "Forbidden";
	_error_map[404] = "Not Found";
	_error_map[405] = "Method Not Allowed";
	_error_map[406] = "Not Acceptable";
	_error_map[407] = "Proxy Authentication Required";
	_error_map[408] = "Request Timeout";
	_error_map[409] = "Conflict";
	_error_map[410] = "Gone";
	_error_map[411] = "Length Required";
	_error_map[412] = "Precondition Failed";
	_error_map[413] = "Payload Too Large";
	_error_map[414] = "URI Too Long";
	_error_map[415] = "Unsupported Media Type";
	_error_map[416] = "Range Not Satisfiable";
	_error_map[417] = "Expectation Failed";
	_error_map[418] = "I'm a teapot";
	_error_map[421] = "Misdirected Request";
	_error_map[422] = "Unprocessable Entity";
	_error_map[423] = "Locked";
	_error_map[424] = "Failed Dependency";
	_error_map[425] = "Too Early";
	_error_map[426] = "Upgrade Required";
	_error_map[428] = "Precondition Required";
	_error_map[429] = "Too Many Requests";
	_error_map[431] = "Request Header Fields Too Large";
	_error_map[451] = "Unavailable For Legal Reasons";
	_error_map[500] = "Internal Server Error";
	_error_map[501] = "Not Implemented";
	_error_map[502] = "Bad Gateway";
	_error_map[503] = "Service Unavailable";
	_error_map[504] = "Gateway Timeout";
	_error_map[505] = "HTTP Version Not Supported";
	_error_map[506] = "Variant Also Negotiates";
	_error_map[507] = "Insufficient Storage";
	_error_map[508] = "Loop Detected";
	_error_map[510] = "Not Extended";
	_error_map[511] = "Network Authentication Required";
}

Response::Response()
	: _status_code(0), _status_message(""), _body(""), _headers(), _request(), _error_map(), _url(""),
	  _path_root(""), _server()
{
	_error_map_init();
}

Response::Response(const Request &request, const server &server)
	: _status_code(0), _status_message(""), _body(""), _headers(), _request(request), _error_map(), _url(""),
	  _path_root(""), _server(server)
{
	_error_map_init();
	
	_path_root = _server.root;
	if (_path_root.empty())
		_path_root = "www/";
	_url = _request.get_url();
	if (_url == "/")
	{
		_url = _server.default_file;
		if (_url.empty())
			_url = "index.html";
	}
}

Response::Response(const Response &response)
{
	if (this != &response)
	{
		_status_code = response._status_code;
		_status_message = response._status_message;
		_body = response._body;
		_headers = response._headers;
		_request = response._request;
		_error_map = response._error_map;
		_url = response._url;
		_path_root = response._path_root;
		_server = response._server;
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
		_error_map = response._error_map;
		_url = response._url;
		_path_root = response._path_root;
		_server = response._server;
	}
	return *this;
}

Response::~Response()
{
}

void Response::generate()
{
	const std::string method = _request.get_method(); 

	if (method == "GET")
		_get();
	else if (method == "POST")
		_post();
	else if (method == "DELETE")
		_delete();
	else
		_status_code = 405;
}
