#include "main.hpp"
#include "logger.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <map>
#include <cstdlib>
#include <ctime>

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

class HttpErrorCodes {
private:
    typedef std::map<int, std::string> ErrorCodeMap;
    ErrorCodeMap httpErrorCodes;

public:
    // Default constructor
    HttpErrorCodes() {
        initializeErrorCodes();
    }

    // Copy constructor
    HttpErrorCodes(const HttpErrorCodes& other) {
        httpErrorCodes = other.httpErrorCodes;
    }

    // Copy assignment operator
    HttpErrorCodes& operator=(const HttpErrorCodes& other) {
        if (this != &other) {
            httpErrorCodes = other.httpErrorCodes;
        }
        return *this;
    }

    // Destructor
    ~HttpErrorCodes() {}

    std::string getDescription(int errorCode) const {
        ErrorCodeMap::const_iterator it = httpErrorCodes.find(errorCode);
        if (it != httpErrorCodes.end()) {
            return it->second;
        }
        return "Unknown Error";
    }

private:
    // Initialize error codes
    void initializeErrorCodes() {
        httpErrorCodes[100] = "Continue";
        httpErrorCodes[101] = "Switching Protocols";
        httpErrorCodes[102] = "Processing";
        httpErrorCodes[200] = "OK";
        httpErrorCodes[201] = "Created";
        httpErrorCodes[202] = "Accepted";
        httpErrorCodes[203] = "Non-Authoritative Information";
        httpErrorCodes[204] = "No Content";
        httpErrorCodes[205] = "Reset Content";
        httpErrorCodes[206] = "Partial Content";
        httpErrorCodes[207] = "Multi-Status";
        httpErrorCodes[208] = "Already Reported";
        httpErrorCodes[226] = "IM Used";
        httpErrorCodes[300] = "Multiple Choices";
        httpErrorCodes[301] = "Moved Permanently";
        httpErrorCodes[302] = "Found";
        httpErrorCodes[303] = "See Other";
        httpErrorCodes[304] = "Not Modified";
        httpErrorCodes[305] = "Use Proxy";
        httpErrorCodes[306] = "(Unused)";
        httpErrorCodes[307] = "Temporary Redirect";
        httpErrorCodes[308] = "Permanent Redirect";
        httpErrorCodes[400] = "Bad Request";
        httpErrorCodes[401] = "Unauthorized";
        httpErrorCodes[402] = "Payment Required";
        httpErrorCodes[403] = "Forbidden";
        httpErrorCodes[404] = "Not Found";
        httpErrorCodes[405] = "Method Not Allowed";
        httpErrorCodes[406] = "Not Acceptable";
        httpErrorCodes[407] = "Proxy Authentication Required";
        httpErrorCodes[408] = "Request Timeout";
        httpErrorCodes[409] = "Conflict";
        httpErrorCodes[410] = "Gone";
        httpErrorCodes[411] = "Length Required";
        httpErrorCodes[412] = "Precondition Failed";
        httpErrorCodes[413] = "Payload Too Large";
        httpErrorCodes[414] = "URI Too Long";
        httpErrorCodes[415] = "Unsupported Media Type";
        httpErrorCodes[416] = "Range Not Satisfiable";
        httpErrorCodes[417] = "Expectation Failed";
        httpErrorCodes[418] = "I'm a teapot";
        httpErrorCodes[421] = "Misdirected Request";
        httpErrorCodes[422] = "Unprocessable Entity";
        httpErrorCodes[423] = "Locked";
        httpErrorCodes[424] = "Failed Dependency";
        httpErrorCodes[425] = "Too Early";
        httpErrorCodes[426] = "Upgrade Required";
        httpErrorCodes[428] = "Precondition Required";
        httpErrorCodes[429] = "Too Many Requests";
        httpErrorCodes[431] = "Request Header Fields Too Large";
        httpErrorCodes[451] = "Unavailable For Legal Reasons";
        httpErrorCodes[500] = "Internal Server Error";
        httpErrorCodes[501] = "Not Implemented";
        httpErrorCodes[502] = "Bad Gateway";
        httpErrorCodes[503] = "Service Unavailable";
        httpErrorCodes[504] = "Gateway Timeout";
        httpErrorCodes[505] = "HTTP Version Not Supported";
        httpErrorCodes[506] = "Variant Also Negotiates";
        httpErrorCodes[507] = "Insufficient Storage";
        httpErrorCodes[508] = "Loop Detected";
        httpErrorCodes[510] = "Not Extended";
        httpErrorCodes[511] = "Network Authentication Required";
    }
};

class Response
{
  private:
    int client_sock;
    std::string request;
    int type;
    HttpErrorCodes hec;

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
        {
            log(ERROR, "filename:" + std::string(strerror(errno)));
            generateHTTPError(404);
        }
        else
        {
            std::string response;
            std::string line;
            while (std::getline(file, line))
                response += line + "\n";
            file.close();

            std::string httpResponse = generateHTTPResponse(response);
            send(client_sock, httpResponse.c_str(), httpResponse.size(), 0);
        }
    }

    void    generateHTTPError(int num)
    {
        std::stringstream ss;
        ss << num;
        std::string reponse;
        std::ifstream file("www/ErrorPage");
        std::string line;
        reponse += "HTTP/1.1 ";
        reponse += ss.str();
        reponse += " ";
        reponse += hec.getDescription(num);
        reponse += "\n\r\n\r";
        while(std::getline(file,line))
            reponse += line + "\n\r";
        file.close();
		std::srand(std::time(0));
		if (std::rand() % 2)
		{
			reponse += "<img src=\"https://http.cat/";
			reponse += ss.str() + "\" alt=\"Centered Image\"\n\r";
			reponse += "width=\"800\"\n\rheight=\"600\"\n\r/>";
			reponse += "</div>\n</body>\n\r</html>\n\r";
		}
		else
		{
			reponse += "<img src=\"https://http.dog/";
			reponse += ss.str() + ".jpg\" alt=\"Centered Image\"\n\r";
			reponse += "width=\"800\"\n\rheight=\"600\"\n\r/>";
			reponse += "</div>\n</body>\n\r</html>\n\r";
		}
        std::cout << reponse << std::endl;

        send(client_sock, reponse.c_str(), reponse.size(), 0);
    }
};
