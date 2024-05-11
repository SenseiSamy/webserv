#include "Response.hpp"

void Response::_error_codes_init()
{
	_error_codes[100] = "Continue";
	_error_codes[101] = "Switching Protocols";
	_error_codes[102] = "Processing";
	_error_codes[200] = "OK";
	_error_codes[201] = "Created";
	_error_codes[202] = "Accepted";
	_error_codes[203] = "Non-Authoritative Information";
	_error_codes[204] = "No Content";
	_error_codes[205] = "Reset Content";
	_error_codes[206] = "Partial Content";
	_error_codes[207] = "Multi-Status";
	_error_codes[208] = "Already Reported";
	_error_codes[226] = "IM Used";
	_error_codes[300] = "Multiple Choices";
	_error_codes[301] = "Moved Permanently";
	_error_codes[302] = "Found";
	_error_codes[303] = "See Other";
	_error_codes[304] = "Not Modified";
	_error_codes[305] = "Use Proxy";
	_error_codes[306] = "(Unused)";
	_error_codes[307] = "Temporary _redirect";
	_error_codes[308] = "Permanent _redirect";
	_error_codes[400] = "Bad Request";
	_error_codes[401] = "Unauthorized";
	_error_codes[402] = "Payment Required";
	_error_codes[403] = "Forbidden";
	_error_codes[404] = "Not Found";
	_error_codes[405] = "Method Not Allowed";
	_error_codes[406] = "Not Acceptable";
	_error_codes[407] = "Proxy Authentication Required";
	_error_codes[408] = "Request Timeout";
	_error_codes[409] = "Conflict";
	_error_codes[410] = "Gone";
	_error_codes[411] = "Length Required";
	_error_codes[412] = "Precondition Failed";
	_error_codes[413] = "Payload Too Large";
	_error_codes[414] = "URI Too Long";
	_error_codes[415] = "Unsupported Media Type";
	_error_codes[416] = "Range Not Satisfiable";
	_error_codes[417] = "Expectation Failed";
	_error_codes[418] = "I'm a teapot";
	_error_codes[421] = "Misdirected Request";
	_error_codes[422] = "Unprocessable Entity";
	_error_codes[423] = "Locked";
	_error_codes[424] = "Failed Dependency";
	_error_codes[425] = "Too Early";
	_error_codes[426] = "Upgrade Required";
	_error_codes[428] = "Precondition Required";
	_error_codes[429] = "Too Many Requests";
	_error_codes[431] = "Request Header Fields Too Large";
	_error_codes[451] = "Unavailable For Legal Reasons";
	_error_codes[500] = "Internal Server Error";
	_error_codes[501] = "Not Implemented";
	_error_codes[502] = "Bad Gateway";
	_error_codes[503] = "Service Unavailable";
	_error_codes[504] = "Gateway Timeout";
	_error_codes[505] = "HTTP Version Not Supported";
	_error_codes[506] = "Variant Also Negotiates";
	_error_codes[507] = "Insufficient Storage";
	_error_codes[508] = "Loop Detected";
	_error_codes[510] = "Not Extended";
	_error_codes[511] = "Network Authentication Required";
}

Response::Response(): _status_code(0), _status_message(""), _body(""), _headers(), _request(), _error_codes(), _url(""), _path_to_root(""), _server()
{
	_error_codes_init();
	_set_root();
	_select_route();
	_generate();
}

Response::Response(const Request &request, const server &server): _status_code(0), _status_message(""), _body(""), _headers(), _request(request), _error_codes(), _url(""), _path_to_root(""), _server(server)
{
	_error_codes_init();
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
		_url = other._url;
		_path_to_root = other._path_to_root;
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
		_url = response._url;
		_path_to_root = response._path_to_root;
		_server = response._server;
	}
	return *this;
}

Response::~Response()
{
}

void Response::_generate()
{
	const std::string method = _request.get_method(); 

	if (method == "GET")
		_get();
	else if (method == "POST")
		_post();
	// else if (method == "DELETE")
	// 	_delete();
	else
		_generate_error(405);
}
