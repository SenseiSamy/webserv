#include "Response.hpp"

/* Functions */
#include <sstream>	// std::ostringstream
#include <unistd.h> // write

Response::Response(Server &server, const Request &request)
		: _server(server), _request(request), _version(request.get_version()), _route(_find_route()),
			_file_path(_real_path(request.get_uri())), _content_length(0), _is_cgi(false)
{
	const std::string &methods = request.get_method();
	if (methods != "GET" && methods != "POST" && methods != "PUT" && methods != "DELETE")
	{
		generate_error_page(405);
		return;
	}

	if (_version != "HTTP/1.0" && _version != "HTTP/1.1")
	{
		generate_error_page(505);
		return;
	}
}

Response::~Response()
{
}

void Response::generate_page()
{
	std::ostringstream response_stream;

	// Write status line
	response_stream << _version << " " << _status_code << " " << _status_message << "\r\n";

	// Write headers
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		response_stream << it->first << ": " << it->second << "\r\n";

	// End headers section
	response_stream << "\r\n";

	// Write body
	response_stream << _body;

	_response_str = response_stream.str();
}

void Response::generate_error_page(const unsigned short &status_code)
{
	std::string status_message;
	const std::map<unsigned short, std::string>::const_iterator it = _server.get_error_codes().find(status_code);
	if (it != _server.get_error_codes().end())
		status_message = it->second;
	else
		status_message = "Unknown Error";

	// Generate a simple HTML error page
	std::ostringstream oss_content;
	oss_content << "<html><head><title>" << status_code << " " << status_message << "</title></head>" << "<body><h1>"
							<< status_code << " " << status_message << "</h1>" << "</body></html>";
	std::string content = oss_content.str();
	_content_type = "text/html";

	_server.set_status_code(status_code);
}

void Response::send_response(const int &client_fd)
{
	send(client_fd, _response_str.c_str(), _response_str.size(), 0);
}
