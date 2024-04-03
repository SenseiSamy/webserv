#include "logger.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
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

class HttpErrorCodes
{
  private:
    typedef std::map<int, std::string> ErrorCodeMap;
    ErrorCodeMap httpErrorCodes;

  public:
    // Default constructor
    HttpErrorCodes()
    {
        initializeErrorCodes();
    }

    // Copy constructor
    HttpErrorCodes(const HttpErrorCodes &other)
    {
        httpErrorCodes = other.httpErrorCodes;
    }

    // Copy assignment operator
    HttpErrorCodes &operator=(const HttpErrorCodes &other)
    {
        if (this != &other)
        {
            httpErrorCodes = other.httpErrorCodes;
        }
        return *this;
    }

    // Destructor
    ~HttpErrorCodes()
    {
    }

    std::string getDescription(int errorCode) const
    {
        ErrorCodeMap::const_iterator it = httpErrorCodes.find(errorCode);
        if (it != httpErrorCodes.end())
        {
            return it->second;
        }
        return "Unknown Error";
    }

  private:
    // Initialize error codes
    void initializeErrorCodes()
    {
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
    
    void parseRequest() 
    {
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

        // Extract headers lines
        std::string line;
        while (std::getline(stream, line))
        {
            std::string headername;
            std::string headervalue;
            size_t  colonPos = line.find_first_of(':');
            if (line.empty() || colonPos == std::string::npos)
                break;
            else
            {
                headername = line.substr(0, colonPos);
                headervalue = line.substr(colonPos + 1);
                while (headername.find('\t') != std::string::npos)
                    headername = headername.substr(headername.find('\t'));
                headers[headername] = headervalue;
            }
        }
        while (std::getline(stream, line))
        {
            body += line + "\n";
        }
    }
};

class   Response    {
    private:
        int statusCode;
		int type;
        HttpErrorCodes hec;
        std::string statusMessage;
        std::string body;
        std::map<std::string, std::string> headers;
		HTTPRequest request;

		void	find_type()
		{
			std::string filename = getRequest().getURL();
			if (filename == "/")
				this->type = HTML;
			else if (filename.find(".html") != std::string::npos)
				this->type = HTML;
			else if (filename.find(".css") != std::string::npos)
				this->type = CSS;
			else if (filename.find(".jpg") != std::string::npos)
				this->type = JPG;
			else if (filename.find(".ico") != std::string::npos)
				this->type = ICO;
			else
				this->type = UNKNOW;
		}

		void	add_content_type()
		{
			if (type == HTML)
				addHeader("Content-Type","text/html");
			else if (type == CSS)
				addHeader("Content-Type","text/css");
			else if (type == JPG)
				addHeader("Content-Type","image/jpeg");
			else if (type == ICO)
				addHeader("Content-Type","image/vnd.microsoft.icon");
		}

		void	generateHTTPError(int num)
		{
			std::ifstream file("www/ErrorPage");
			std::string line;
			setStatusCode(num);
			setStatusMessage(hec.getDescription(num));
			while (std::getline(file, line))
				body += line + "\n";
			file.close();
			std::srand(std::time(0));
			std::stringstream ss;
			ss << num;
			int randnum = std::rand();
			if (randnum % 4 == 0)
			{
				body += "<img src=\"https://http.cat/";
				body += ss.str() + "\" alt=\"Centered Image\"\n";
				body += "width=\"800\"\nheight=\"600\"\n/>";
				body += "</div>\n</body>\n</html>\n\r";
			}
			else if (randnum % 4 == 1)
			{
				body += "<img src=\"https://http.dog/";
				body += ss.str() + ".jpg\" alt=\"Centered Image\"\n";
				body += "width=\"800\"\nheight=\"600\"\n/>";
				body += "</div>\n</body>\n</html>\n\r";
			}

			else if (randnum % 4 == 2)
			{
				body += "<img src=\"https://http.pizza/";
				body += ss.str() + ".jpg\" alt=\"Centered Image\"\n";
				body += "width=\"800\"\nheight=\"600\"\n/>";
				body += "</div>\n</body>\n</html>\n\r";
			}
			else
			{
				body += "<img src=\"https://httpgoats.com/";
				body += ss.str() + ".jpg\" alt=\"Centered Image\"\n";
				body += "width=\"800\"\nheight=\"600\"\n/>";
				body += "</div>\n</body>\n</html>\n\r";
			}
		}

		void	add_content_lenght()
		{
			std::stringstream ss;
			ss << body.length();
			addHeader("Content-Lenght", ss.str());
		}

		void	Get_handler()
		{
			find_type();
			std::string filename = "www" + getRequest().getURL();
			if (getRequest().getURL() == "/")
				filename = "www/index.html";
			std::ifstream file(filename.c_str());
			if (file.is_open())
			{
				setStatusCode(200);
				setStatusMessage(hec.getDescription(200));
				add_content_type();
				std::string line;
				while (std::getline(file, line))
					body += line + "\n";
				add_content_type();
				add_content_lenght();
			}
			else
				generateHTTPError(404);
		}

    public:

        // Constructor with parameters
        Response(HTTPRequest req) : request(req){
				generate_response();
            }

        // Copy constructor
        Response(const Response& other)
            : statusCode(other.statusCode), statusMessage(other.statusMessage), body(other.body), headers(other.headers), request(other.request) {

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
		~Response() {

		}

		

		void	generate_response()
		{
			if (getRequest().getMethod() == "GET")
				Get_handler();
			else if (getRequest().getMethod() == "POST")
			{
				std::string boundaries = request.getHeader("Content-Type");
				size_t boundpos = boundaries.find("boundary=");
				if (boundpos != std::string::npos)
				{
					boundaries = boundaries.substr(boundpos + 9);
                    // std::cout << boundaries << std::endl;
					std::istringstream data(this->request.getBody());
                    std::string filename;
                    std::string line;
                    int tokenheader = 2;
                    while (std::getline(data, line) && tokenheader)
                    {
                        // std::cout << "ceci est une line :" << line << "\t tokenheader = " << tokenheader << std::endl;
                        if (line.find(boundaries))
                        {
                            // std::cout << "ici boundaries" << std::endl;
                        }
                        if (line.find("filename=") != std::string::npos)
                        {
                            filename = line.substr(line.find("filename=") + 10);
                            tokenheader--;
                        }
                        else if (line.find("Content-Type:") != std::string::npos)
                            tokenheader--;
                        if (tokenheader == 0)
                            break;
                        
                    }
                    filename = filename.substr(0,filename.size() - 2);
                    // std::cout << filename << std::endl;
                    std::cout << boundaries << std::endl;
                    std::getline(data, line);
                    std::ofstream file(filename.c_str(), std::ios::binary);
                    while (std::getline(data, line))
                    {
                        std::string test = line;
                        if (test.find(boundaries) != std::string::npos)
                            break;
                        file << line + "\n";
                    }
                    file.close();
				}
			}
		}

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

		const HTTPRequest & getRequest() const {
			return request;
		}

        // Method to convert response to string
        std::string toString() const
		{
			std::stringstream ss;
			ss << statusCode;
			std::stringstream dd;
			dd << body.length();
            std::string response = "HTTP/1.1 ";
            response += ss.str() + " " + statusMessage + "\r\n";


            // Append headers
            for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            response += it->first + ": " + it->second + "\r\n";
            }

            response += "\r\n";
            response += body;
            return response;
        }
};