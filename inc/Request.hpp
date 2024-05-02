#pragma once

#include <map>
#include <string>

class Request
{
  private:
    std::string _request;
    std::string _method;
    std::string _url;
    std::map<std::string, std::string> _headers;
    size_t _content_lenght;
    std::string _body;
	std::string _query_string;

    void parseRequest();

  public:
    Request(const std::string& request);
    Request(const Request& other);
    ~Request();
    Request& operator=(const Request& other);

    const std::string& get_request() const;
    const std::string& get_method() const;
    const std::string& get_url() const;
    const std::map<std::string, std::string>& get_headers() const;
    const std::string& get_headers_key(const std::string& key) const;
    size_t get_content_lenght() const;
    const std::string& get_body() const;

    void display_request() const;
    void display_headers() const;
};
