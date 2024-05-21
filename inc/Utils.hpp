#ifndef UTILS_HPP
#define UTILS_HPP

/* Types */
#include <map>		// std::map
#include <string> // std::string, size_t
#include <vector> // std::vector

/* Functions */
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

const std::map<unsigned short, std::string> init_error_codes();
const std::vector<std::string> split(const std::string &str, char delim);

#endif // UTILS_HPP