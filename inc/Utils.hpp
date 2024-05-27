#ifndef UTILS_HPP
#define UTILS_HPP

/* Types */
#include <map>		// std::map
#include <string> // std::string, size_t
#include <vector> // std::vector

/* Functions */
#include <cstring> // std::strcpy
#include <sstream> // std::ostringstream, std::istringstream

template <typename T> std::string to_string(const T &value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

template <typename T> size_t to_size_t(const T &str)
{
	std::istringstream iss(str);
	size_t value;
	iss >> value;
	return value;
}

template <typename T> T *T_dup(const T &str)
{
	T *new_str = new T[str.size()];

	for (size_t i = 0; i < str.size(); i++)
		new_str[i] = str[i];

	return (new_str);
}

char **to_cstr_array(const std::map<std::string, std::string> &map);
const std::map<unsigned short, std::string> init_error_codes();
const std::vector<std::string> split(const std::string &str, char delim);

#endif // UTILS_HPP