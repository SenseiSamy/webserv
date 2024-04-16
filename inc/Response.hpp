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
#include "HTTPErrorCodes.hpp"
#include "HTTPRequest.hpp"
#include "Server.hpp"

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
		server_data server;

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
        Response(HTTPRequest req, server_data &serv) : request(req), server(serv) {
				generate_response();
            }

        // Copy constructor
        Response(const Response& other)
            : statusCode(other.statusCode), statusMessage(other.statusMessage), body(other.body), headers(other.headers), request(other.request), server(other.server) {

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