#include "Response.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>

#define BUFFER_SIZE 5000
#define EMPTY_BYTES 13

Response::Response(Request req, server_data& serv) : _request(req), _server(serv)
{
    generate_response();
}

Response& Response::operator=(const Response& other)
{
    if (this != &other)
    {
        this->_request = other._request;
        this->_server = other._server;
    }
    return *this;
}

Response::~Response()
{
}

const std::string Response::to_string() const
{
    std::stringstream ss;
    ss << _status_code;
    std::stringstream dd;
    dd << _body.length();
    std::string response = "HTTP/1.1 ";
    response += ss.str() + " " + _status_message + "\r\n";

    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
        response += it->first + ": " + it->second + "\r\n";

    response += "\r\n";
    response += _body;
    return response;
}

void Response::find_type()
{
    std::string filename = get_request().get_url();
    if (filename == "/")
        this->_type = HTML;
    else if (filename.find(".html") != std::string::npos)
        this->_type = HTML;
    else if (filename.find(".css") != std::string::npos)
        this->_type = CSS;
    else if (filename.find(".jpg") != std::string::npos)
        this->_type = JPG;
    else if (filename.find(".ico") != std::string::npos)
        this->_type = ICO;
    else
        this->_type = UNKNOW;
}

void Response::add_content_type()
{
    if (_type == HTML)
        set_headers("Content-Type", "text/html");
    else if (_type == CSS)
        set_headers("Content-Type", "text/css");
    else if (_type == JPG)
        set_headers("Content-Type", "image/jpeg");
    else if (_type == ICO)
        set_headers("Content-Type", "image/vnd.microsoft.icon");
}

void Response::generateHTTPError(int num)
{
    std::ifstream file("www/ErrorPage");
    std::string line;
    setStatusCode(num);
    setStatusMessage(_hec.get_description(num));
    while (std::getline(file, line))
        _body += line + "\n";
    file.close();
    std::srand(time(0));
    std::stringstream ss;
    ss << num;
    int randnum = std::rand();
    if (randnum % 4 == 0)
    {
        _body += "<img src=\"https://http.cat/";
        _body += ss.str() + "\" alt=\"Centered Image\"\n";
        _body += "width=\"800\"\nheight=\"600\"\n/>";
        _body += "</div>\n</body>\n</html>\n\r";
    }
    else if (randnum % 4 == 1)
    {
        _body += "<img src=\"https://http.dog/";
        _body += ss.str() + ".jpg\" alt=\"Centered Image\"\n";
        _body += "width=\"800\"\nheight=\"600\"\n/>";
        _body += "</div>\n</body>\n</html>\n\r";
    }

    else if (randnum % 4 == 2)
    {
        _body += "<img src=\"https://http.pizza/";
        _body += ss.str() + ".jpg\" alt=\"Centered Image\"\n";
        _body += "width=\"800\"\nheight=\"600\"\n/>";
        _body += "</div>\n</body>\n</html>\n\r";
    }
    else
    {
        _body += "<img src=\"https://httpgoats.com/";
        _body += ss.str() + ".jpg\" alt=\"Centered Image\"\n";
        _body += "width=\"800\"\nheight=\"600\"\n/>";
        _body += "</div>\n</body>\n</html>\n\r";
    }
}

void Response::get_handler()
{
    Response::find_type();
    std::string filename = "www" + get_request().get_url();
    if (get_request().get_url() == "/")
        filename = "www/index.html";
    std::ifstream file(filename.c_str());
    if (file.is_open())
    {
        setStatusCode(200);
        setStatusMessage(_hec.get_description(200));
        std::string line;
        while (std::getline(file, line))
            _body += line + "\n";
        add_content_type();
        set_content_lenght();
    }
    else
        generateHTTPError(404);
}

void Response::post_handler()
{
    if (this->get_request().get_headers().empty())
    {
        this->generateHTTPError(400);
        return ;
    }
    std::string boundaries = _request.get_headers_key("Content-Type");
    size_t boundpos = boundaries.find("boundary=");
    if (boundpos != std::string::npos)
    {
        boundaries = boundaries.substr(boundpos + 9);
        std::istringstream data(this->_request.get_body());
        std::string filename;
        std::string line;
        size_t	bodyread = 0;
        int tokenheader = 2;
        while (std::getline(data, line) && tokenheader)
        {
            if (line.find(boundaries) != std::string::npos)						
                bodyread += line.length();
            else if (line.find("filename=") != std::string::npos)
            {
                filename = line.substr(line.find("filename=") + 10);
                tokenheader--;
                bodyread += line.length();
            }
            else if (line.find("Content-Type:") != std::string::npos)
            {
                tokenheader--;
                bodyread += line.length();
            }
            if (tokenheader == 0)
                break;
        }
        if (filename.empty() || tokenheader)
        {
            this->generateHTTPError(400);
            return ;
        }
        //filename.size - 2 pour enlever le retour a la ligne
        filename = filename.substr(0, filename.size() - 2);
        //skipping empty line
        std::getline(data, line);
        std::ofstream file(filename.c_str(), std::ios::binary);
        char	buff[BUFFER_SIZE];
        size_t	contentadd = 1;
        while (BUFFER_SIZE * contentadd < _request.get_content_lenght())
        {
            data.read(buff, BUFFER_SIZE);
            file.write(buff, BUFFER_SIZE);
            // this->_body.append(buff, BUFFER_SIZE);
            contentadd++;
        }
        data.read(buff, _request.get_content_lenght() - (BUFFER_SIZE * (contentadd - 1) + (bodyread + boundaries.length() + EMPTY_BYTES)));
        file.write(buff, _request.get_content_lenght() - (BUFFER_SIZE * (contentadd - 1) + (bodyread + boundaries.length() + EMPTY_BYTES)));
        // this->_body.append(buff, _request.get_content_lenght() - (BUFFER_SIZE * (contentadd - 1) + (bodyread + boundaries.length() + EMPTY_BYTES)));
        file.close();
        setStatusCode(201);
        setStatusMessage(_hec.get_description(201));
        add_content_type();
        // set_content_lenght();
    }
    else
        this->generateHTTPError(400);
}

void Response::generate_response()
{
    if (get_request().get_method() == "GET")
        Response::get_handler();
    else if (get_request().get_method() == "POST")
        Response::post_handler();
}