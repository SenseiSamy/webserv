#ifndef PARSING_HPP
#define PARSING_HPP

#include "Server.hpp"

/* Types */
#include <string> // std::string
#include <vector> // std::vector

/* Functions */
#include <cstring>

extern size_t index_line;
extern size_t index_word;
extern std::vector<std::vector<std::string> > content_file;

/* Parsing/Index.cpp */
const std::string next_word();
const std::string previous_word();

/* Parsing/Parsing.cpp*/
const std::vector<std::string> get_value(const std::string &token);
std::vector<Server> parsing_config(const std::string &config_file, const std::map<unsigned short, std::string> &error_codes);

/* Parsing/Syntax.cpp*/
void route_value(route &route, const std::string &word);
void server_value(server &server, const std::string &word, const std::map<unsigned short, std::string> &error_codes);
void syntax_brackets();

#endif // PARSING_HPP
