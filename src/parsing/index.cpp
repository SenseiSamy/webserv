#include "Parsing.hpp"

static size_t next_non_empty_line()
{
	size_t i = index_line + 1;
	while (i < content_file.size() && content_file[i].empty())
		++i;
	return i;
}

const std::string next_word()
{
	if (index_line >= content_file.size())
		return "";
	if (index_word >= content_file[index_line].size())
	{
		if (next_non_empty_line() >= content_file.size())
			return "";
		while (content_file[++index_line].empty())
			;
		index_word = 0;
	}
	return content_file[index_line][index_word++];
}

const std::string previous_word()
{
	if (index_line == 0 && index_word == 0)
		return "";
	if (index_word == 0)
	{
		while (content_file[--index_line].empty())
			;
		index_word = content_file[index_line].size();
	}
	return content_file[index_line][--index_word];
}
