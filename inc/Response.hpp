#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "Server.hpp"
#include <map>
#include <string>

enum Method
{
	GET = 1,
	POST,
	DELETE
};

enum type
{
	HTML,
	CSS,
	JPG,
	ICO,
	UNKNOW
};

class Response
{
	private:
		// Response
		int _status_code;
		int _type;
		std::string _status_message;
		std::string _body;
		std::map<std::string, std::string> _headers;

		// Cgi
		std::map<std::string, std::string> _meta_var;

		// Utils
		Request _request;
		std::map<unsigned int, std::string> _error_codes;
		std::string _uri;
		std::string _path_to_root;
		server _server;
		route* _route;
		bool _is_cgi;

		// cgi
		void _init_meta_var();
		char** _map_to_env();
		int _fork_and_exec(int* fd, int& pid, std::string path_to_exec_prog);
		std::string _get_cgi_output(int* fd);
		int _cgi_request(std::string &rep, std::string path_to_exec_prog);

		void _find_type();
		void _add_content_type();
		void _generate_error(int num);
		void _select_route();
		void _set_root();
		int _check_and_rewrite_uri();
		bool _is_a_directory(const std::string &uri) const;
		bool _is_a_file(const std::string &uri) const;
		bool _exists (const std::string& name) const;
		bool _write_perm(const std::string& name) const;
		void _redirect();
		void _directory_listing();

		int _cgi();
		void _post();
		void _get();
		void _delete();

		void _generate();

	public:
		Response();
		Response(const Request &request, const server &server, const std::map<unsigned int, std::string> &error_codes);
		Response(const Response &other);
		Response &operator=(const Response &other);
		~Response();

		// getters
		const int &get_status_code() const;
		const int &get_type() const;
		const std::map<unsigned int, std::string> &get_error_codes() const;
		const std::string &get_status_message() const;
		const std::string &get_body() const;
		const std::map<std::string, std::string> &get_headers() const;
		const std::string get_headers_key(const std::string &key) const;
		const Request &get_request() const;
		const server &get_server() const;

		// setters
		void set_status_code(int code);
		void set_status_message(const std::string &message);
		void setBody(const std::string &responseBody);
		void set_headers(const std::string &key, const std::string &value);
		void set_content_lenght();

		// displays
		void display() const;

		const std::string convert() const;
};

#endif // RESPONSE_HPP