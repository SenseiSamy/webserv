#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "Server.hpp"

/* Types */
#include <cstddef> // size_t
#include <fstream> // std::ifstream
#include <map>
#include <string>

class Response
{
private:
	Server &_server;
	const Request &_request;
	std::string _response_str;

	/* data */
	const std::string &_version;
	std::string _status_code;
	std::string _status_message;
	std::map<std::string, std::string> _headers;
	std::string _body;

	/* buffer */

	/* Additional variables */
	const route *_route;
	std::string _file_path;
	size_t _content_length;
	std::string _content_type;
	std::ifstream _file_stream;

	/* CGI variables */
	bool _is_cgi;
	std::string _cgi_script_path;
	std::string _cgi_output;

	/* Utils */
	const std::string _real_path(const std::string &path);
	void _set_content_type(const std::string &file_extension);
	void _set_status(const std::string &status_code, const std::string &status_message);
	void _read_file_content(const std::string &file_path);
	void _execute_cgi_script();
	const route *_find_route() const;

public:
	Response(Server &server, const Request &request);
	~Response();

	/* Getters */
	const Server &get_server() const;
	const Request &get_request() const;

	/* Setters */

	/* Methods */
	void generate_page();
	void generate_error_page(const unsigned short &status_code);

	void send_response(const int &client_fd);
};

#endif // RESPONSE_HPP
