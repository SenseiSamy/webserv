#ifndef SERVER_HPP
#define SERVER_HPP

/* Types */
#include <cerrno>				// errno
#include <map>					// std::map
#include <netinet/in.h> // struct sockaddr_in
#include <string>				// std::string
#include <sys/types.h>	// size_t
#include <vector>				// std::vector

/* Functions */
#include <cstring>	// strerror
#include <unistd.h> // close

#define MAX_EVENTS 64
#define MAX_BUFFER_SIZE 4096

/* Classes */
class Request;

typedef struct route
{
	// required
	std::string path; // /kapouet

	// optional
	std::vector<std::string> accepted_methods; // GET
	std::string root;													 // /tmp/www
	std::string redirect;											 // /var/www/html
	std::string rewrite;											 // /var/www/html
	bool directory_listing;										 // true
	std::string default_file;									 // index.html

	/// cgi
	std::map<std::string, std::string> cgi; // .php /usr/bin/php-cgi
	std::string cgi_upload_path;						// /tmp/upload
	bool cgi_upload_enable;									// true
} route;

typedef struct server
{
	struct sockaddr_in addr;
	int listen_fd;

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
} server;

class Server
{
private:
	bool _verbose;
	int _epoll_fd;
	const std::map<unsigned short, std::string> _error_codes;
	std::vector<server> _servers;
	std::map<int, Request> _requests;

	/* Parsing */
	std::string _config_file;
	std::map<size_t, std::vector<std::string> > _content_file;
	size_t _current_word;
	size_t _current_line;

	size_t _next_non_empty_line();
	void _reset_index();
	const std::string _next_word();
	const std::string _previous_word();
	const std::vector<std::string> _get_value(const std::string &token);
	void _route_value(route &result, const std::string &word);
	void _server_value(server &server, const std::string &word);
	void _syntax_brackets();
	void _read_config();
	const server _parse_server();
	const route _parse_route();
	void _parsing_config();

	/* Server */
	void _setup_server_socket(server &server);
	bool _accept_new_connection(server *server);
	void _read_request(int fd);

public:
	Server();
	Server(const char *config_file, bool verbose);
	Server(const Server &other);
	~Server();
	Server &operator=(const Server &other);

	// Getters
	const bool &get_verbose() const;
	const int &get_epoll_fd() const;
	const std::map<unsigned short, std::string> &get_error_codes() const;
	const std::vector<server> &get_servers() const;
	const std::map<int, Request> &get_requests() const;
	const std::string &get_config_file() const;
	const std::map<size_t, std::vector<std::string> > &get_content_file() const;
	const size_t &get_current_word() const;
	const size_t &get_current_line() const;

	// Setters
	void set_verbose(const bool &verbose);
	void set_epoll_fd(const int &epoll_fd);
	void set_error_codes(const std::map<unsigned short, std::string> &error_codes);
	void set_servers(const std::vector<server> &servers);
	void set_requests(const std::map<int, Request> &requests);
	void set_config_file(const std::string &config_file);
	void set_content_file(const std::map<size_t, std::vector<std::string> > &content_file);
	void set_current_word(const size_t &current_word);
	void set_current_line(const size_t &current_line);

	// Displays
	void display() const;

	// Others
	std::string to_string(size_t i) const;

	void run();
};

#endif // SERVER_HPP
