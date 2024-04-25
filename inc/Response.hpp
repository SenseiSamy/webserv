#pragma once

#include "ErrorCodes.hpp"
#include "Request.hpp"
#include "Server.hpp"

#include <map>
#include <string>

#define GET 1
#define POST 2
#define DELETE 3

enum type
{
    HTML,
    CSS,
    JPG,
    ICO,
    UNKNOW
};

class Response
{
  private:
    int _status_code;
    int _type;
    ErrorCodes _hec;
    std::string _status_message;
    std::string _body;
    std::map<std::string, std::string> _headers;
    Request _request;
    server_data _server;

    void find_type();
    void add_content_type();
    void generateHTTPError(int num);
    void post_handler();
    void get_handler();

  public:
    Response(Request req, server_data& serv);
    Response& operator=(const Response& other);
    ~Response();

    // getters
    const int& get_status_code() const;
    const int& get_type() const;
    const ErrorCodes& get_hec() const;
    const std::string& get_status_message() const;
    const std::string& get_body() const;
    const std::map<std::string, std::string>& get_headers() const;
    const std::string& get_headers_key(const std::string& key) const;
    const Request& get_request() const;
    const server_data& get_server() const;

    // setters
    void setStatusCode(int code);
    void setStatusMessage(const std::string& message);
    void setBody(const std::string& responseBody);
    void set_headers(const std::string& key, const std::string& value);
    void set_content_lenght();

    const std::string to_string() const;
    void generate_response();
};