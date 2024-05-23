#ifndef SERVER_HPP
#define SERVER_HPP

/* Types */
#include <cerrno>				// errno
#include <map>					// std::map
#include <netinet/in.h> // sockaddr_in
#include <string>				// std::string
#include <vector>				// std::vector

/* Functions */
#include <cstdlib>	// exit
#include <cstring>	// strerror
#include <stdio.h>	// perror
#include <unistd.h> // close

class Request;
class Response;

#define MAX_EVENTS 64
#define BUFFER_SIZE 1024

struct route
{
	// required
	std::string path; // /kapouet

	// optional
	std::vector<std::string> accepted_methods; // GET
	std::string root;													 // /tmp/www
	std::string redirect;											 // /var/www/html
	bool directory_listing;										 // true
	std::string default_file;									 // index.html

	/// cgi
	std::map<std::string, std::string> cgi; // .php /usr/bin/php-cgi
	std::string cgi_upload_path;						// /tmp/upload
	bool cgi_upload_enable;									// true

	route() : directory_listing(true), cgi_upload_enable(true)
	{
	}
};

struct server
{
	// required
	std::string host;		 // 127.0.0.1
	unsigned short port; // 8080

	// optional
	std::string root;																// /var/www/html
	std::string default_file;												// index.html
	std::vector<std::string> server_names;					// example.com
	bool default_server;														// true
	size_t max_body_size;														// 1000000
	std::map<std::string, std::string> error_pages; // 404 /404.html
	std::vector<route> routes;											// /kapouet

	server()
			: host("127.0.0.1"), port(8080), root("www/"), default_file("index.html"), default_server(true),
				max_body_size(1000000)
	{
	}
};

class Server
{
private:
	int _server_fd;
	struct sockaddr_in _address;

	const server _server;
	const std::map<unsigned short, std::string> &_error_codes;

	unsigned short _status_code;

	/* Methods */
	/* HTTP Methods */
	void _delete(const Request &request, Response &response);
	void _get(const Request &request, Response &response);
	void _post(const Request &request, Response &response);
	void _put(const Request &request, Response &response);

	inline const std::string _real_path(const std::string &path);

	/* Utils */
	void _set_nonblocking(int sockfd);
	int _bind_socket(const std::string &ip, int port);

public:
	Server(const server &serv, const std::map<unsigned short, std::string> &error_codes);
	~Server();

	/* Getters */
	const int &get_server_fd() const;
	const server &get_server() const;
	const std::map<unsigned short, std::string> &get_error_codes() const;
	const unsigned short &get_status_code() const;

	/* Setters */
	void set_status_code(const unsigned short &status_code);

	/* Methods */
	void init();
	void accept_connection(int epoll_fd);
	void handle_client(int client_fd);
};

#endif // SERVER_HPP