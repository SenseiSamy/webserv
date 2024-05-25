#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Server.hpp"

/* Functions */
#include <fstream> // std::ifstream

/* Types */
#include <cstddef> // size_t
#include <map>
#include <string>

class Request
{
private:
	Server &_server;
	const int _client_fd;
	std::string _request;

	/* data */
	std::string _method;
	std::string _uri;
	std::string _version;
	std::map<std::string, std::string> _headers;

	enum State
	{
		START,
		HEADERS,
		BODY,
		COMPLETE,
		ERROR
	};

	State _state;
	std::ofstream _tmp_body;
	size_t _content_length;

	/* Methods */
	void parse_headers();
	void parse_request_line();

public:
	Request(Server &server, const char *request, const int &client_fd);
	~Request();

	/* Getters */
	const int &get_client_fd() const;
	const std::string &get_request() const;
	const std::string &get_method() const;
	const std::string &get_uri() const;
	const std::string &get_version() const;
	const std::map<std::string, std::string> &get_headers() const;
	const std::string &get_body() const;

	/* Setters */
	void set_method(const std::string &method);
	void set_uri(const std::string &uri);
	void set_version(const std::string &version);
	void set_headers(const std::map<std::string, std::string> &headers);
	void set_body(const std::string &body);

	/* Methods */
	void concatenate_request(const std::string &request);
	bool is_complet();
	void parse_request();
};

#endif // REQUEST_HPP