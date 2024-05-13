#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

std::string concatenate_paths(const std::string& base, const std::string& relative)
{
	std::vector<std::string> base_tokens;
	std::vector<std::string> relative_tokens;
	std::vector<std::string> result_tokens;

	std::istringstream base_stream(base);
	std::string token;
	while (std::getline(base_stream, token, '/'))
	{
		if (!token.empty())
			base_tokens.push_back(token);
	}

	std::istringstream relative_stream(relative);
	while (std::getline(relative_stream, token, '/'))
	{
		if (!token.empty())
			relative_tokens.push_back(token);
	}

	for (std::vector<std::string>::const_iterator it = base_tokens.begin(); it != base_tokens.end(); ++it)
	{
		if (*it == "..")
		{
			if (!result_tokens.empty())
				result_tokens.pop_back();
		}
		else
			result_tokens.push_back(*it);
	}

	for (std::vector<std::string>::const_iterator it = relative_tokens.begin(); it != relative_tokens.end(); ++it)
	{
		if (*it == "..")
		{
			if (!result_tokens.empty())
				result_tokens.pop_back();
		}
		else
			result_tokens.push_back(*it);
	}

	std::string result;
	for (std::vector<std::string>::const_iterator it = result_tokens.begin(); it != result_tokens.end(); ++it)
		result += *it + '/';

	return result;
}

// Example usage:
int main()
{
	std::string base = "www";
	std::string relative = "/../../";
	std::cout << "Concatenated Path: " << concatenate_paths(base, relative) << std::endl;

	base = "www/";
	relative = "////";
	std::cout << "Concatenated Path: " << concatenate_paths(base, relative) << std::endl;

	return 0;
}
