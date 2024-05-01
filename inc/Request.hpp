#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>

class Request
{
  private:
    int _client_fd;
    std::string _request;

    std::string _method;
    std::string _uri;
    std::map<std::string, std::string> _header;
    size_t _content_length;
    std::string _body;

  public:
    Request();
    Request(int listen_fd, const std::string &request);
    Request(const Request &request);
    ~Request();
    Request &operator=(const Request &request);

    // Getters
    int get_client_fd() const;
    std::string get_request() const;
    std::string get_method() const;
    std::string get_uri() const;
    std::map<std::string, std::string> get_header() const;
    std::string get_header_key(const std::string &key) const;
    size_t get_content_length() const;
    std::string get_body() const;

    // Setters
    void set_request(const std::string &request);
    void set_method(const std::string &method);
    void set_uri(const std::string &uri);
    void set_header(const std::map<std::string, std::string> &header);
    void set_header_key(const std::string &key, const std::string &value);
    void set_content_length(const size_t content_length);
    void set_body(const std::string &body);

    // Displays
    std::ostream &operator<<(std::ostream &os) const;

    // Others    
    void clear();
    void parse_request_line();
    void parse();
};

#endif // REQUEST_HPP