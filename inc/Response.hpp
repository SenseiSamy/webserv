#include "main.hpp"
#include "logger.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>

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
    int client_sock;
    std::string request;
    int type;

    void logError(const std::string &message)
    {
        std::cerr << "Error: " << message << std::endl;
    }

  public:
    // Constructor
    Response(int sock, const char *req)
    {
        client_sock = sock;
        request = req;
    }

    // Destructor
    ~Response()
    {
        close(client_sock);
    }

    // Copy constructor
    Response(const Response &other)
    {
        client_sock = other.client_sock;
        request = other.request;
    }

    // Assignment operator
    Response &operator=(const Response &other)
    {
        if (this != &other)
        {
            client_sock = other.client_sock;
            request = other.request;
        }
        return *this;
    }

    void sendResponse()
    {
        std::string filename = extractFilename();
        std::ifstream file(filename.c_str());
        if (!file.is_open())
            log(ERROR, "filename:" + std::string(strerror(errno)));
        std::string response;
        std::string line;
        while (std::getline(file, line))
            response += line + "\n";
        file.close();

        std::string httpResponse = generateHTTPResponse(response);
        send(client_sock, httpResponse.c_str(), httpResponse.size(), 0);
    }

    std::string extractFilename()
    {
        std::string filename;
        size_t first_space = request.find_first_of(' ');
        size_t second_space = request.find_first_of(' ', first_space + 1);
        if (first_space != std::string::npos && second_space != std::string::npos)
            filename = "www" + request.substr(first_space + 1, second_space - first_space - 1);
        if (request.substr(first_space + 1, second_space - first_space - 1) == "/")
            filename = "www/index.html";
        if (filename.find(".html") != std::string::npos)
            this->type = HTML;
        else if (filename.find(".css") != std::string::npos)
            this->type = CSS;
        else if (filename.find(".jpg") != std::string::npos)
            this->type = JPG;
        else if (filename.find(".ico") != std::string::npos)
            this->type = ICO;
        else
            this->type = UNKNOW;
        return filename;
    }

    std::string generateHTTPResponse(const std::string &content)
    {
        std::string httpResponse;
        if (content.empty())
            httpResponse = "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n";
        else
        {
            if (type == HTML)
                httpResponse = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + content;
            else if (type == CSS)
                httpResponse = "HTTP/1.1 200 OK\nContent-Type: text/css\n\n" + content;
            else if (type == JPG)
                httpResponse = "HTTP/1.1 200 OK\nContent-Type: image/jpeg\n\n" + content;
            else if (type == ICO)
                httpResponse = "HTTP/1.1 200 OK\nContent-Type: image/vnd.microsoft.icon\n\n" + content;
            else
                httpResponse = "HTTP/1.1 200 OK\n\n" + content; // default to plain text
        }
        return httpResponse;
    }
};
