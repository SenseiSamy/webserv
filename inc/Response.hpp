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
    int _status_code;
    int _type;
    std::string _status_message;
    std::string _body;
    std::map<std::string, std::string> _header;

    Request _request;
    std::map<unsigned short, std::string> _error_map;
    std::string _uri;
    std::string _path_root;
    std::map<std::string, std::string> _meta_var;

    server _server;
    route *_route;
    bool _is_cgi;

    // Cgi
    char **_map_to_env();
    void _generate_meta_variables();
    int _fork_and_exec(int *fd, int &pid, std::string path_to_root, std::string path_to_exec_prog, std::string url);
    std::string _get_cgi_output(int *fd);
    int _cgi(std::string &rep, std::string url, std::string path_to_root, std::string path_to_exec_prog);
    bool _is_cgi_request();

    int _check_file_uri();
    void _redirect();
    void _set_content_length();
    void _directory_listing();
    void _http_error(unsigned short status_code);
    void _mime_type();
    int _get();
    int _post();
    int _delete();

  public:
    Response();
    Response(const Request &request, const server &server);
    Response(const Response &response);
    Response &operator=(const Response &response);
    ~Response();

    // Getters
    int get_status_code() const;
    int get_type() const;
    std::string get_status_message() const;
    std::string get_body() const;
    std::map<std::string, std::string> get_header() const;
    Request get_request() const;
    std::map<unsigned short, std::string> get_error_map() const;
    std::string get_uri() const;
    std::string get_path_root() const;
    server get_server() const;

    // Setters
    void set_status_code(const int status_code);
    void set_type(const int type);
    void set_status_message(const std::string &status_message);
    void set_body(const std::string &body);
    void set_header(const std::map<std::string, std::string> &header);
    void set_request(const Request &request);
    void set_error_map(const std::map<unsigned short, std::string> &error_map);
    void set_uri(const std::string &uri);
    void set_path_root(const std::string &path_root);
    void set_server(const server &server);

    // Displays
    void display() const;

    // Others

    std::string to_string(size_t i) const;
    void send_response();
};

#endif // RESPONSE_HPP