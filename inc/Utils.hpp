#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>

template <typename T> std::string to_string(const T &value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

template <typename T> size_t to_size_t(const T &value)
{
	std::istringstream is(value);
	size_t result;
	is >> result;
	return result;
}

#endif // UTILS_HPP