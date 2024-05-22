#include "Response.hpp"
#include "Utils.hpp"

Response::Response(Server &server, const Request &request)
		: _server(server), _request(request), _response_buffer(NULL), _response_size(0)
{
	const std::string &methods = _server.get_methods();

	if (methods == "GET")
		_get(request);
	else if (methods == "HEAD")
		_head(request);
	else if (methods == "POST")
		_post(request);
	else if (methods == "PUT")
		_put(request);
	else if (methods == "DELETE")
		_delete(request);
	else if (methods == "CONNECT")
		_connect(request);
	else if (methods == "OPTIONS")
		_options(request);
	else if (methods == "TRACE")
		_trace(request);
	else
		generate_error_page(501);

	update_response_buffer();
	send(_request.get_client_fd(), _response_buffer, _response_size, 0);
}

Response::~Response()
{
	delete[] _response_buffer;
}

void Response::update_response_buffer()
{
	_response_size = _version.size() + 1 + _status_code.size() + 1 + _status_message.size() + 1;
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		_response_size += it->first.size() + 2 + it->second.size() + 2;
	_response_size += 2 + _body.size() + 2;

	_response_buffer = new char[_response_size + 1];
	std::string response = _version + " " + _status_code + " " + _status_message + "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		response += it->first + ": " + it->second + "\r\n";
	response += "\r\n" + _body;

	std::copy(response.begin(), response.end(), _response_buffer);
	_response_buffer[_response_size] = '\0';
}

void Response::generate_error_page(const unsigned short &status_code)
{
	_version = _server.get_version();
	_status_code = to_string(status_code);
	_status_message = _server.get_error_codes().at(status_code);

	_headers["Content-Type"] = "text/html";
	_headers["Content-Length"] = "0";

	_body = "<html><head><title>" + _status_code + " " + _status_message + "</title></head><body><center><h1>" +
					_status_code + " " + _status_message + "</h1></center><hr><center>Webserver</center></body></html>";
}