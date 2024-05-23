#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Server.hpp"

/* Types */
#include <map>
#include <string>

class Request
{
private:
	Server &_server;
	const int _client_fd;
	const std::string _request;

	/* data */
	std::string _method;
	std::string _uri;
	std::string _version;
	std::map<std::string, std::string> _headers;
	std::string _body;

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
	void parse_request(const std::string &request);
};

#endif // REQUEST_HPP