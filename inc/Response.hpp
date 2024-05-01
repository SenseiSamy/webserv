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

    server _server;

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

    // Setters

    // Displays
    std::ostream &operator<<(std::ostream &os) const;

    // Others
    std::string to_string(size_t i) const;
    int send_response();
};

#endif // RESPONSE_HPP