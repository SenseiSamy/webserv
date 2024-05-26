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
	std::string _stash;

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
	size_t _content_received;
	size_t _content_length;

	/* Methods */
	void parse_headers(const std::string::size_type &end);
	void parse_request_line(const std::string::size_type &end);

public:
	Request(Server &server, const int &client_fd);
	~Request();

	/* Getters */
	const Server &get_server() const;
	const int &get_client_fd() const;
	const std::string &get_request() const;
	const std::string &get_method() const;
	const std::string &get_uri() const;
	const std::string &get_version() const;
	const std::map<std::string, std::string> &get_headers() const;

	/* Setters */
	void set_method(const std::string &method);
	void set_uri(const std::string &uri);
	void set_version(const std::string &version);
	void set_headers(const std::map<std::string, std::string> &headers);

	/* Methods */
	void concatenate_request(const std::string &request);
	bool is_complet() const;
};

std::ostream &operator<<(std::ostream &os, const Request &request);

#endif // REQUEST_HPP