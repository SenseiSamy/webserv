#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>

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

		void printHeaders() const
		{
			for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
                std::cout << it->first << ": " << it->second << std::endl;
            }
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