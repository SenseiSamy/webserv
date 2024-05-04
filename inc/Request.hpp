#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>

class Request
{
  private:
    const std::string _request;
    std::string _method;
    std::string _uri;
    std::map<std::string, std::string> _header;
    size_t _content_length;
    std::string _body;
    std::string _http_version;
    std::string _query_string;

  public:
    Request();
    Request(int listen_fd, const std::string &request);
    Request(const Request &request);
    ~Request();
    Request &operator=(const Request &request);

    // Getters
    std::string get_method() const;
    std::string get_uri() const;
    std::string get_header(const std::string &key);
    std::map<std::string, std::string> get_header() const;
    size_t get_content_length() const;
    std::string get_body() const;
    std::string get_http_version() const;
    std::string get_query_string() const;

    // Setters

    // Displays
    void display_request() const;
    void display() const;

    // Others    
    void clear();
    void parse_request_line();
    void parse();
};

#endif // REQUEST_HPP