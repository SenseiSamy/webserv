#include "Response.hpp"
#include "Server.hpp"
#include <cstring>
#include <linux/limits.h>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <sstream>

Response::Response(Request req, server_data& serv) : _request(req), _server(serv)
{
	select_route();
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

void Response::select_route()
{
	const std::string request = _request.get_url();
	for (std::size_t i = 0; i < _server.route.size(); ++i) {
		if (request.find(_server.route[i].root) == 0) {
			for (std::size_t j = 0; j < _server.route[i].methods.size(); ++j) {
				if (_request.get_method() == _server.route[i].methods[j]) {
					_route = &_server.route[i];
					return;
				}
			}
		}
	}
	_route = NULL;
}

void Response::generate_response()
{
	if (get_request().get_method() == "GET")
		Response::get_handler();
	else if (get_request().get_method() == "POST")
	{
		std::string boundaries = _request.get_headers_key("Content-Type");
		size_t boundpos = boundaries.find("boundary=");
		if (boundpos != std::string::npos)
		{
			boundaries = boundaries.substr(boundpos + 9);
			// std::cout << boundaries << std::endl;
			std::istringstream data(this->_request.get_body());
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
			filename = filename.substr(0, filename.size() - 2);
			// std::cout << filename << std::endl;
			std::cout << boundaries << std::endl;
			std::getline(data, line);
			std::ofstream file(filename.c_str(), std::ios::binary);
			char buff[1];
			while (!data.eof())
			{
				data.read(buff, 1);
				file.write(buff, 1);
			}
			file.close();
		}
	}
	else
		generateHTTPError(405);
}