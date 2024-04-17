#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <iostream>
# include <map>
# include <sstream>
# include <string>
# include <cstdlib>
# include <ctime>

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
		size_t	content_lenght;
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
        if (!method.compare("POST"))
		{
			std::stringstream temp;
			temp << headers["Content-Lenght"];
			temp >> content_lenght;
			size_t	count = 0;
			char buff[1];
			while (count < content_lenght && !stream.eof())
			{
				stream.read(buff, 1);
				body.push_back(buff[0]);
				count++;
			}
		}
    }
};

#endif