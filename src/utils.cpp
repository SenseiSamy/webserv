#include "Utils.hpp"

const std::map<unsigned short, std::string> init_error_codes()
{
	std::map<unsigned short, std::string> error_codes;

	error_codes[100] = "Continue";
	error_codes[101] = "Switching Protocols";
	error_codes[102] = "Processing";
	error_codes[200] = "OK";
	error_codes[201] = "Created";
	error_codes[202] = "Accepted";
	error_codes[203] = "Non-Authoritative Information";
	error_codes[204] = "No Content";
	error_codes[205] = "Reset Content";
	error_codes[206] = "Partial Content";
	error_codes[207] = "Multi-Status";
	error_codes[208] = "Already Reported";
	error_codes[226] = "IM Used";
	error_codes[300] = "Multiple Choices";
	error_codes[301] = "Moved Permanently";
	error_codes[302] = "Found";
	error_codes[303] = "See Other";
	error_codes[304] = "Not Modified";
	error_codes[305] = "Use Proxy";
	error_codes[306] = "(Unused)";
	error_codes[307] = "Temporary Redirect";
	error_codes[308] = "Permanent Redirect";
	error_codes[400] = "Bad Request";
	error_codes[401] = "Unauthorized";
	error_codes[402] = "Payment Required";
	error_codes[403] = "Forbidden";
	error_codes[404] = "Not Found";
	error_codes[405] = "Method Not Allowed";
	error_codes[406] = "Not Acceptable";
	error_codes[407] = "Proxy Authentication Required";
	error_codes[408] = "Request Timeout";
	error_codes[409] = "Conflict";
	error_codes[410] = "Gone";
	error_codes[411] = "Length Required";
	error_codes[412] = "Precondition Failed";
	error_codes[413] = "Payload Too Large";
	error_codes[414] = "URI Too Long";
	error_codes[415] = "Unsupported Media Type";
	error_codes[416] = "Range Not Satisfiable";
	error_codes[417] = "Expectation Failed";
	error_codes[418] = "I'm a teapot";
	error_codes[421] = "Misdirected Request";
	error_codes[422] = "Unprocessable Entity";
	error_codes[423] = "Locked";
	error_codes[424] = "Failed Dependency";
	error_codes[425] = "Too Early";
	error_codes[426] = "Upgrade Required";
	error_codes[428] = "Precondition Required";
	error_codes[429] = "Too Many Requests";
	error_codes[431] = "Request Header Fields Too Large";
	error_codes[451] = "Unavailable For Legal Reasons";
	error_codes[500] = "Internal Server Error";
	error_codes[501] = "Not Implemented";
	error_codes[502] = "Bad Gateway";
	error_codes[503] = "Service Unavailable";
	error_codes[504] = "Gateway Timeout";
	error_codes[505] = "HTTP Version Not Supported";
	error_codes[506] = "Variant Also Negotiates";
	error_codes[507] = "Insufficient Storage";
	error_codes[508] = "Loop Detected";
	error_codes[510] = "Not Extended";
	error_codes[511] = "Network Authentication Required";

	return error_codes;
}

const std::vector<std::string> split(const std::string &str, char delim)
{
	std::vector<std::string> tokens;
	std::string token;

	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == delim)
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
		}
		else
			token += str[i];
	}

	if (!token.empty())
	{
		tokens.push_back(token);
		token.clear();
	}
	return tokens;
}
