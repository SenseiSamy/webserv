#include "Response.hpp"

/* Functions */
#include <limits.h> // PATH_MAX
#include <sstream>	// std::ostringstream
#include <unistd.h> // write

Response::Response(Server &server, const Request &request)
		: _server(server), _request(request), _version(request.get_version()), _route(_find_route()),
			_real_path_root(_real_path(_server.get_server().root)), _content_length(0),
			_file_type(_get_file_type(_request.get_uri())), _is_cgi(false)
{
	_headers["Server"] = "webserv";
	_headers["Connection"] = "close";

	if (_real_path_root.empty() && !_server.has_error())
		_server.set_status_code(403);
	else if (_real_path_root.find(_server.get_server().root) != 0 && !_server.has_error())
		_server.set_status_code(404);

	if (_server.has_error())
		generate_error_page(_server.get_status_code());
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
	(void)send(client_fd, _response.c_str(), _response.size(), 0);
}
