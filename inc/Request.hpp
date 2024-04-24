#pragma once

#include <map>
#include <string>

class Request
{
  private:
    std::string _original_request;

    std::string _method;
    std::string _uri;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;

    void parse_request(const std::string& request);

  public:
    Request(const std::string& request);
    ~Request();

    /* Getters */
    const std::string& get_method() const;
    const std::string& get_uri() const;
    const std::string& get_version() const;
    const std::map<std::string, std::string>& get_header() const;
    const std::string get_header_value(const std::string& headerName) const;
    const std::string& get_body() const;

    /* display */
    void display_original_request(int port, const std::string& host) const;
    void display_request(int port, const std::string& host) const;
};
