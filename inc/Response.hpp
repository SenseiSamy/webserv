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
	char *_response_buffer;
	size_t _response_size;

	/* Methods */
	// Outils pour generer les methodes get post etc (par exemple: get_extention, set_mime, cgi etc)
	// Donc en gros la Reponse est senser englober tout les outils utiliser par les methodes get post etc.
	// Oui c'est inutile en soit, mais ca permet de garder la class Server plus propre et lisible.


public:
	Response(Server &_server, const Request &request);
	~Response();

	/* Getters */
	const Server &get_server() const;
	const Request &get_request() const;
	const char *get_response_buffer() const;
	const size_t &get_response_size() const;

	/* Setters */
	void set_response_buffer(const char *response_buffer);
	void set_response_size(const size_t &response_size);
};

#endif // RESPONSE_HPP