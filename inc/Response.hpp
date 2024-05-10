#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "Server.hpp"

#include <cstddef>
#include <map>
#include <string>

class Response
{
  private:
    Request _request;
    std::map<unsigned short, std::string> _error_map;
    std::string _url;
    std::string _path_root;
    std::map<std::string, std::string> _meta_var;

    std::string _path;
    int _status_code;
    int _type;
    std::string _status_message;
    std::string _body;
    std::map<std::string, std::string> _headers;

    server _server;
    route *_route;
    bool _is_cgi;

    void _error_map_init();
    std::string _to_string(size_t i) const;

    // Utils
    std::string _concatenate_paths(const std::string &base, const std::string &relative);
    std::string _sanitize_url(const std::string &url);
    const std::string _mime_type(const std::string &file) const;
    int _is_directory_or_file(const std::string &path) const;
    void _generate_response_body(unsigned short error_code);
    void _generate_response(const std::string &path);

    // Handlers
    void _get();
    void _put();
    void _post();
    void _delete();

  public:
    Response();
    Response(const Request &request, const server &server);
    Response(const Response &response);
    Response &operator=(const Response &response);
    ~Response();

    // Getters

    // Setters

    // Displays
    void display() const;

    // Others
    void generate();
};

#endif // RESPONSE_HPP