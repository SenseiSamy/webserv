#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Server.hpp"

#include <fstream>
#include <netinet/in.h>

enum state
{
	incomplete,
	header_complete,
	complete,
	invalid
};

class Request
{
private:
	/* Request composents */
	std::string _request;
	std::string _method;
	std::string _uri;
	std::string _version;
	std::map<std::string, std::string> _headers;
	std::string _body;

	/* Other */
	std::vector<server> _servers;
	server _server;
	size_t _content_length;
	std::string _query_string;
	enum state _state;
	std::fstream _tmp_file;
	size_t _file_size;
	std::string _file_name;
	std::string _client_addr;

	void _refresh_state();
	server _find_server(const std::string &host) const;

public:
	Request();
	Request(const std::string &request, const std::vector<server> &servers);
	Request(const Request &other);
	~Request();
	Request &operator+=(const std::string &str);

	// Getters
	const std::vector<server> &get_servers() const;
	const server &get_server() const;
	const std::string &get_request() const;
	const std::string &get_method() const;
	const std::string &get_uri() const;
	const std::string &get_version() const;
	const std::map<std::string, std::string> &get_headers() const;
	const std::string get_headers_key(const std::string &key) const;
	const size_t &get_content_length() const;
	const std::string &get_body() const;
	const std::string &get_query_string() const;
	const enum state &get_state() const;
	const std::fstream &get_tmp_file() const;
	const size_t &get_file_size() const;
	const std::string &get_file_name() const;
	const std::string get_first_line() const;
	const std::string &get_client_addr() const;


	// Setters
	void set_servers(const std::vector<server> &servers);
	void set_server(const server &server);
	void set_request(const std::string &request);
	void set_method(const std::string &method);
	void set_uri(const std::string &uri);
	void set_version(const std::string &version);
	void set_headers(const std::map<std::string, std::string> &headers);
	void set_headers_key(const std::string &key, const std::string &value);
	void set_content_length(const size_t &content_length);
	void set_body(const std::string &body);
	void set_query_string(const std::string &query_string);
	void set_state(const enum state &state);
	void set_file_size(const std::size_t &file_size);
	void set_file_name(const std::string &file_name);
	void set_client_addr(const std::string &addr);

	// Displays
	void display_request_str() const;
	void display() const;

	// Others
	void clear();
	void parse();
	bool http100_continue;
};

#endif // REQUEST_HPP
