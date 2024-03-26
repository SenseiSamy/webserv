#include "main.hpp"
#include "logger.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <map>

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

class   HTTPRequest {
    public:
        HTTPRequest(const std::string& request) : request(request) {
            parseRequest();
        }
    
        HTTPRequest(const HTTPRequest& other) : request(other.request) {
            parseRequest();
        }
    
        ~HTTPRequest() {
        
        }
    
        std::string getMethod() const {
            return method;
        }
    
        std::string getURL() const {
            return url;
        }
    
        std::string getHeader(const std::string& name) const {
            std::map<std::string, std::string>::const_iterator it = headers.find(name);
            if (it != headers.end()) {
                return it->second;
            }
            return "";
        }
    
        std::string getBody() const {
            return body;
        }
    
        void printRequest() const {
            std::cout << "Method: " << method << std::endl;
            std::cout << "URL: " << url << std::endl;
            std::cout << "Headers:\n";
            for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
                std::cout << it->first << ": " << it->second << std::endl;
            }
            std::cout << "Body: " << body << std::endl;
        }
    
    private:
        std::string request;
        std::string method;
        std::string url;
        std::map<std::string, std::string> headers;
        std::string body;
    
        void parseRequest() {
            std::istringstream stream(request);
            std::string requestLine;
            if (!std::getline(stream, requestLine)) {
                // Invalid request format
                return;
            }
    
            // Extract request line
            std::istringstream requestLineStream(requestLine);
            if (!std::getline(requestLineStream, method, ' ') || !std::getline(requestLineStream, url, ' ')) {
                // Invalid request line format
                return;
            }
    
            // Parse headers
            std::string headerLine;
            while (std::getline(stream, headerLine) && !headerLine.empty()) {
                size_t colonPos = headerLine.find(':');
                if (colonPos != std::string::npos) {
                    std::string headerName = headerLine.substr(0, colonPos);
                    std::string headerValue = headerLine.substr(colonPos + 1); // Skip the colon
                    // Remove leading whitespace from the value
                    size_t valueStart = headerValue.find_first_not_of(" \t");
                    if (valueStart != std::string::npos) {
                        headerValue = headerValue.substr(valueStart);
                    }
                    headers[headerName] = headerValue;
                }
            }
    
            // Parse body
            std::stringstream bodyStream;
            bodyStream << stream.rdbuf();
            body = bodyStream.str();
        }
};

class   Response    {
    private:
        int statusCode;
        HttpErrorCodes hec;
        std::string statusMessage;
        std::string body;
        std::map<std::string, std::string> headers;

    public:
        // Default constructor
        Response() : statusCode(200), statusMessage("OK"), body("Default Response") {
            
        }

        // Constructor with parameters
        Response(int code, const std::string& message, const std::string& responseBody)
            : statusCode(code), statusMessage(message), body(responseBody) {

            }

        // Copy constructor
        Response(const Response& other)
            : statusCode(other.statusCode), statusMessage(other.statusMessage), body(other.body), headers(other.headers) {

            }

        // Copy assignment operator
        Response& operator=(const Response& other) {
            if (this != &other) {
                statusCode = other.statusCode;
                statusMessage = other.statusMessage;
                body = other.body;
                headers = other.headers;
            }
            return *this;
        }

        // Destructor (default is fine for this class)

        // Setter methods
        void setStatusCode(int code) {
            statusCode = code;
        }

        void setStatusMessage(const std::string& message) {
            statusMessage = message;
        }

        void setBody(const std::string& responseBody) {
            body = responseBody;
        }

        void addHeader(const std::string& name, const std::string& value) {
            headers[name] = value;
        }

        // Getter methods
        int getStatusCode() const {
            return statusCode;
        }

        const std::string& getStatusMessage() const {
            return statusMessage;
        }

        const std::string& getBody() const {
            return body;
        }

        const std::map<std::string, std::string>& getHeaders() const {
            return headers;
        }

        // Method to convert response to string
        std::string toString() const {
            std::string response = "HTTP/1.1 ";
            response += std::to_string(statusCode) + " " + statusMessage + "\r\n";
            response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
            response += "Content-Type: text/plain\r\n";

            // Append headers
            for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            response += it->first + ": " + it->second + "\r\n";
            }

            response += "\r\n";
            response += body;
            return response;
        }
};