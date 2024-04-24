#include "Request.hpp"

#include <sstream>

Request::Request(const std::string& request) : _original_request(request)
{
    parse_request(_original_request);
}

Request::~Request()
{
}

void Request::parse_request(const std::string& request)
{
    std::istringstream request_stream(request);
    std::string line;

    std::getline(request_stream, line);

    std::istringstream line_stream(line);
    line_stream >> _method >> _uri >> _version;

    // Lire les en-têtes
    while (std::getline(request_stream, line) && line != "\r")
    {
        std::size_t pos = line.find(':');
        if (pos != std::string::npos)
        {
            std::string name = line.substr(0, pos);
            std::string value = line.substr(pos + 2); // +2 pour sauter ": "
            _headers[name] = value;
        }
    }

    // Lecture du corps de la requête (optionnel)
    while (std::getline(request_stream, line))
        _body += line;
}