#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"

/* Types */
#include <cstddef> // size_t

class Response
{
private:
	Server &_server;
	const Request &_request;
	std::string _path;

	/* data */
	std::string _version;
	std::string _status_code;
	std::string _status_message;
	std::map<std::string, std::string> _headers;
	std::string _body;

	/* buffer */
	char *_response_buffer;
	size_t _response_size;

	/* Utils */

public:
	Response(Server &server, const Request &request);
	~Response();

	/* Getters */
	const Server &get_server() const;
	const Request &get_request() const;
	const char *get_response_buffer() const;
	const size_t &get_response_size() const;

	/* Setters */
	void set_response_buffer(const char *response_buffer);
	void set_response_size(const size_t &response_size);

	/* Methods */
	size_t length() const;
	void generate_error_page(const unsigned short &status_code);
	void update_response_buffer();

	void send_response(const int &client_fd);
};

#endif // RESPONSE_HPP