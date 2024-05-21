#include "Parsing.hpp"
#include "Server.hpp"
#include "Utils.hpp"

#include <fstream> // std::ifstream

static const std::vector<std::string> split_line(const std::string &str)
{
	std::vector<std::string> words;
	std::string word;
	bool in_quote = false;

	for (std::string::size_type i = 0; i < str.size(); ++i)
	{
		if (str[i] == '"' || str[i] == '\'')
			in_quote = !in_quote;

		if (!in_quote && str[i] == '#')
			break;

		if (in_quote)
		{
			word += str[i];
			continue;
		}

		if (std::isspace(str[i]) || str[i] == '=' || str[i] == ';' || str[i] == '{' || str[i] == '}')
		{
			if (!word.empty())
			{
				words.push_back(word);
				word.clear();
			}
			if (!std::isspace(str[i]))
				words.push_back(std::string(1, str[i]));
		}
		else
			word += str[i];
	}
	if (!word.empty())
		words.push_back(word);
	return words;
}

static void read_config(const std::string &config_file)
{
	std::ifstream file(config_file.c_str());
	if (!file.is_open())
		throw std::runtime_error("Failed to open file : " + std::string(strerror(errno)));

	size_t line_number = 0;
	std::string line;
	while (std::getline(file, line))
	{
		++line_number;
		std::vector<std::string> words = split_line(line);
		if (words.empty())
			content_file[line_number] = std::vector<std::string>();
		else
			for (std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it)
				content_file[line_number].push_back(*it);
	}

	file.close();
}

const std::vector<std::string> get_value(const std::string &token)
{
	std::vector<std::string> result;

	if (next_word() != "=")
		throw std::runtime_error("Syntax error: Expected '=' after " + token + " at line " + to_string(index_line));

	std::string word = next_word();
	while (word != "" && word != ";")
	{
		if ((word[0] == '"' && word[word.size() - 1] == '"') || (word[0] == '\'' && word[word.size() - 1] == '\''))
			result.push_back(word.substr(1, word.size() - 2));
		else
		{
			if (word != ";")
				throw std::runtime_error("Syntax error: Expected ';' after value at line " + to_string(index_line));
			else
				throw std::runtime_error("Syntax error: Missing \" or ' at line " + to_string(index_line));
		}
		word = next_word();
	}

	if (result.empty())
		throw std::runtime_error("Syntax error: Expected value after " + token + " at line " + to_string(index_line));

	if (word != ";")
		throw std::runtime_error("Syntax error: Expected ';' after value at line " + to_string(index_line));

	return result;
}

static const route parse_route()
{
	route result;

	std::string word = next_word();
	while (word != "" && word != "}")
	{
		route_value(result, word);
		word = next_word();
	}

	if (word != "}")
		throw std::runtime_error("Syntax error: Missing '}' at line " + to_string(index_line));

	if (result.path.empty())
		throw std::runtime_error("Syntax error: Missing path at line " + to_string(index_line));

	return result;
}

static const Server parse_server(const std::map<unsigned short, std::string> &error_codes)
{
	server result;

	std::string word = next_word();
	while (word != "" && word != "}")
	{
		if (word == "route")
		{
			const std::string next = next_word();
			if (next != "{")
				throw std::runtime_error("Syntax error: Expected '{' after route at line " + to_string(index_line));
			result.routes.push_back(parse_route());
		}
		else
			server_value(result, word, error_codes);
		word = next_word();
	}

	if (word != "}")
		throw std::runtime_error("Syntax error: Missing '}' at line " + to_string(index_line));

	return Server(result);
}

std::vector<Server> parsing_config(const std::string &config_file,
																	 const std::map<unsigned short, std::string> &error_codes)
{
	std::vector<Server> servers;
	read_config(config_file);
	index_line = 0;
	index_word = 0;
	std::string word = next_word();

	while (true)
	{
		if (word == "server")
		{
			std::string next = next_word();
			if (next != "{")
				throw std::runtime_error("Syntax error: Expected '{' after server at line " + to_string(index_line));
			servers.push_back(parse_server(error_codes));
		}
		else
			throw std::runtime_error("Syntax error: Invalid keyword " + word + " at line " + to_string(index_line));
		word = next_word();
		if (word == "")
			break;
	}

	return servers;
}
