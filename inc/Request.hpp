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

	/* data */
	std::map<std::string, std::string> _headers;
	std::string _body;

public:
	Request(Server &server);
	~Request();

	/* Getters */
	const std::map<std::string, std::string> &get_headers() const;
	const std::string &get_body() const;

	/* Setters */
	void set_headers(const std::map<std::string, std::string> &headers);
	void set_body(const std::string &body);

	/* Methods */
	void parse_request(const std::string &request);
};

#endif // REQUEST_HPP