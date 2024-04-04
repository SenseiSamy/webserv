#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <ctime>
#include "HTTPRequest.hpp"
#include "Config.hpp"
#include "HttpErrorCodes.hpp"

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

class   Response    {
    private:
        int statusCode;
		int type;
        HttpErrorCodes hec;
        std::string statusMessage;
        std::string body;
        std::map<std::string, std::string> headers;
		HTTPRequest request;
		Server server;

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

		Route*	is_a_route() {
			for (std::size_t i = 0; i < server.routes.size(); ++i) {
				if (server.routes[i].root.compare(getRequest().getURL()) == 0)
					return (&server.routes[i]);
			}
			return (NULL);
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
        Response(HTTPRequest req, Server& serv) : request(req), server(serv) {
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
				// std::cout << request. << std::endl;
				// request.printHeaders();
				std::string boundaries = request.getHeader("Content-Type");
				size_t boundpos = boundaries.find("boundary=");
				if (boundpos != std::string::npos)
				{
					boundaries = boundaries.substr(boundpos + 9);
					std::string data = request.getBody();
					// std::cout << data << std::endl;
					size_t filestart = data.find("\r\n\r\n") + 4;
					size_t fileend = data.find("\r\n--" + boundaries, filestart);
					if (filestart != std::string::npos && fileend != std::string::npos)
					{
						std::string file = data.substr(filestart, fileend - filestart);
					}
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