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
	std::string _response;

	/* data */
	const std::string &_version;
	std::string _status_code;
	std::string _status_message;
	std::map<std::string, std::string> _headers;
	std::string _body;

	/* Additional variables */

	const route *_route;
	std::string _real_path_root;
	std::string _file_path;
	std::string _real_path_file;
	size_t _content_length;
	int _file_type;
	std::string _content_type;
	std::ifstream _file_stream;

	/* CGI variables */
	bool _is_cgi;
	std::string _cgi_script_path;
	std::string _cgi_output;
	std::map<std::string, std::string> _env;

	/* Utils */
	const std::string _real_path(const std::string &path);
	int _get_file_type(const std::string &file_path) const;
	void _set_content_type(const std::string &file_extension);
	void _set_content_length();
	void _set_status(const std::string &status_code, const std::string &status_message);
	void _read_file_content(const std::string &file_path);
	const route *_find_route() const;

	/* Cgi */
	int _is_cgi_script(const std::string &file_path) const;
	void _cgi_upload(const std::string &file_path);
	void _cgi(const std::string &file_path);

	/* Methods */
	void _get(const Request &request);
	void _head(const Request &request);
	void _post(const Request &request);
	void _put(const Request &request);
	void _delete(const Request &request);

public:
	Response(Server &server, const Request &request);
	~Response();

	/* Getters */
	const Server &get_server() const;
	const Request &get_request() const;
	const std::string &get_version() const;
	const std::string &get_response_str() const;
	const std::string &get_status_code() const;
	const std::string &get_status_message() const;
	const std::map<std::string, std::string> &get_headers() const;
	const std::string &get_body() const;
	const route *get_route() const;
	const std::string &get_file_path() const;
	const size_t &get_content_length() const;
	const int &get_file_type() const;
	const std::string &get_content_type() const;
	const std::ifstream &get_file_stream() const;
	const bool &get_is_cgi() const;
	const std::string &get_cgi_script_path() const;
	const std::string &get_cgi_output() const;

	/* Setters */
	void set_status_code(const std::string &status_code);
	void set_status_message(const std::string &status_message);
	void set_header(const std::string &key, const std::string &value);
	void set_body(const std::string &body);
	void set_file_path(const std::string &file_path);
	void set_content_length(const size_t &content_length);
	void set_content_type(const std::string &content_type);
	void set_is_cgi(const bool &is_cgi);
	void set_cgi_script_path(const std::string &cgi_script_path);
	void set_cgi_output(const std::string &cgi_output);

	/* Methods */
	void generate_page();
	void generate_error_page(const unsigned short &status_code);
	void send_response(const int &client_fd);
};

#endif // RESPONSE_HPP
