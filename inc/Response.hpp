#include "main.hpp"
#define GET 1
#define POST 2
#define DELETE 3

class Response {
private:
    int client_sock;
    std::string request;
    std::string log_file;

    void logError(const std::string& message) {
        std::cerr << "Error: " << message << std::endl;
    }

public:
    // Constructor
    Response(int sock, const char* req, const std::string& log) {
        client_sock = sock;
        request = req;
        log_file = log;
    }

    // Destructor
    ~Response() {
        close(client_sock);
    }

    // Copy constructor
    Response(const Response& other) {
        client_sock = other.client_sock;
        request = other.request;
        log_file = other.log_file;
    }

    // Assignment operator
    Response& operator=(const Response& other) {
        if (this != &other) {
            client_sock = other.client_sock;
            request = other.request;
            log_file = other.log_file;
        }
        return *this;
    }

    void sendResponse() {
        std::string filename = extractFilename();
        std::ifstream file(filename.c_str());
        if (!file.is_open()) {
            logError("file open error: " + std::string(strerror(errno)));
            return;
        }

        std::string response;
        std::string line;
        while (std::getline(file, line))
            response += line + "\n";
        file.close();

        std::string httpResponse = generateHTTPResponse(response);
        send(client_sock, httpResponse.c_str(), httpResponse.size(), 0);
    }

    std::string extractFilename() {
        std::string filename = "www/" + request.substr(5, request.find_first_of("HTTP/1.1", 5));
        while (filename.find(' ') != std::string::npos)
            filename.erase(filename.find(' '));
        return filename;
    }

    std::string generateHTTPResponse(const std::string& content) {
        std::string httpResponse;
        if (content.empty()) {
            httpResponse = "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n";
        } else {
            if (request.find(".html") != std::string::npos)
                httpResponse = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + content;
            else if (request.find(".css") != std::string::npos)
                httpResponse = "HTTP/1.1 200 OK\nContent-Type: text/css\n\n" + content;
            else if (request.find(".jpg") != std::string::npos)
                httpResponse = "HTTP/1.1 200 OK\nContent-Type: image/jpeg\n\n" + content;
            else if (request.find(".ico") != std::string::npos)
                httpResponse = "HTTP/1.1 200 OK\nContent-Type: image/vnd.microsoft.icon\n\n" + content;
            else
                httpResponse = "HTTP/1.1 200 OK\n\n" + content; // default to plain text
        }
        return httpResponse;
    }
};