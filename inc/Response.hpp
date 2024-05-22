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

	/* data */
	std::string _version;
	std::string _status_code;
	std::string _status_message;
	std::map<std::string, std::string> _headers;
	std::string _body;

	char *_response_buffer;
	size_t _response_size;

	/*   HTTP Methods */
	void _get(const Request &request);
	void _head(const Request &request);
	void _post(const Request &request);
	void _put(const Request &request);
	void _delete(const Request &request);
	void _connect(const Request &request);
	void _options(const Request &request);
	void _trace(const Request &request);

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
	void generate_error_page(const unsigned short &status_code);
	void update_response_buffer();
};

#endif // RESPONSE_HPP