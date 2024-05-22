#ifndef PARSING_HPP
#define PARSING_HPP

#include "Server.hpp"

class Parsing
{
private:
	static size_t index_line;
	static size_t index_word;
	static std::vector<std::vector<std::string> > content_file;

	/* Index */
	static size_t next_non_empty_line();
	static const std::string next_word();
	static const std::string previous_word();

	/* Parsing */
	static const std::vector<std::string> split_line(const std::string &line);
	static void read_config(const std::string &config_file);
	static const std::vector<std::string> get_value(const std::string &token);
	static const route parse_route();
	static const Server parse_server(const std::map<unsigned short, std::string> &error_codes);

	/* Syntax */
	static void route_value(route &route, const std::string &word);
	static void server_value(server &server, const std::string &word, const std::map<unsigned short, std::string> &error_codes);
	static void syntax_brackets();

public:
	static std::vector<Server> parsing_config(const std::string &config_file,
																		 const std::map<unsigned short, std::string> &error_codes);
};

#endif // PARSING_HPP
