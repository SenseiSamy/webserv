#pragma once

#include "Request.hpp"
#include "Server.hpp"

#include <map>
#include <string>

class Response
{
  private:
    Request _req;
    server_data _server;

    std::string _http_version;
    int _status_code;
    std::map<std::string, std::string> _headers;
    std::string _body;

    static std::string get_status_message(int status_code);

    static std::string get_mime_type(const std::string& filename);
    /* Post functions */
    std::string get_boundary(const std::string& header);
    bool parse_multipart_form_data(const std::string& data, const std::string& boundary,
                                   std::vector<std::string>& files);

    /* Delete functions */

  public:
    Response(const Request& req, const server_data& server);
    ~Response();

    /* Getters */
    const std::string& get_http_version() const;
    int get_status_code() const;
    const std::map<std::string, std::string>& get_headers() const;
    const std::string& get_body() const;

    /* Setters */
    void set_header(const std::string& key, const std::string& value);
    void set_body(const std::string& body);

    std::string serialize() const;

    // Handling different HTTP methods
    void handle_get();
    void handle_post();
    void handle_delete();

    void send_response(int client_fd);
};
