#ifndef HTTPERRORCODES_HPP
# define HTTPERRORCODES_HPP

# include <map>
# include <string>

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

#endif