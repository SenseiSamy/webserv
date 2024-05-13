#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>

class Request
{
	private:
		std::string _request;

		std::string _method;
		std::string _url;
		std::map<std::string, std::string> _headers;
		size_t _content_length;
		std::string _body;
		std::string _query_string;

	public:
		Request();
		Request(const std::string &request);
		Request(const Request &request);
		~Request();
		Request &operator=(const Request &request);

		// Getters
		std::string get_request() const;
		std::string get_method() const;
		std::string get_url() const;
		std::string get_headers_key(const std::string &key);
		std::map<std::string, std::string> get_headers() const;
		size_t get_content_length() const;
		std::string get_body() const;
		std::string get_query_string() const;
		std::string get_first_line() const;
		// Setters

		// Displays
		void display_request() const;
		void display() const;

		// Others    
		void clear();
		void parse();
};

#endif // REQUEST_HPP