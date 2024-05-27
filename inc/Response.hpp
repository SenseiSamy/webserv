#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "Server.hpp"

/* Types */
#include <map>				 // std::map
#include <string>			 // std::string
#include <sys/types.h> // pid_t

class Response
{
private:
	Server &_server;
	const Request &_request;
	const route *_route;
	std::string _final_response;

	/* data */
	const std::string &_version;
	std::string _status_code;
	std::string _status_message;
	std::map<std::string, std::string> _headers;
	std::string _body;

	/* Additional */
	std::string _real_path_root;
	std::string _path_file;
	std::string _real_path_file;

	/* CGI */
	bool _is_cgi;
	std::string _cgi_script_path;
	std::string _cgi_output;
	std::map<std::string, std::string> _env;

	/* Utils */
	/*   Cgi */
	void _init_env();
	int _exec_cgi(int *pipe_fd, pid_t &pid, const std::string &path_to_exec_prog);
	void _get_cgi_output(int *pipe_fd);
	void _cgi_handle();
	void _cgi(const std::string &file_path);

	/*   Other */
	int _get_file_type(const std::string &file_path) const;
	void _set_content_type(const std::string &file_extension);
	void _set_content_length();
	void _set_status(const std::string &status_code, const std::string &status_message);
	void _read_file_content(const std::string &file_path);
	const route *_find_route() const;

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
	const std::string get_headers_key(const std::string &key) const;

	/* Setters */

	/* Methods */
	void generate_page();
	void generate_error_page(const unsigned short &status_code);
	void send_response(const int &client_fd);
};

#endif // RESPONSE_HPP
