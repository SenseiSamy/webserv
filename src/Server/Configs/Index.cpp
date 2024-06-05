#include "Server.hpp"

void Server::_reset_index()
{
	_current_word = 0;
	_current_line = 0;
}

size_t Server::_next_non_empty_line()
{
	size_t i = _current_line + 1;
	while (i < _content_file.size() && _content_file[i].empty())
		++i;
	return i;
}

const std::string Server::_next_word()
{
	if (_current_line >= _content_file.size())
		return "";
	if (_current_word >= _content_file[_current_line].size())
	{
		if (_next_non_empty_line() >= _content_file.size())
			return "";
		while (_content_file[++_current_line].empty())
			;
		_current_word = 0;
	}
	return _content_file[_current_line][_current_word++];
}

const std::string Server::_previous_word()
{
	if (_current_line == 0 && _current_word == 0)
		return "";
	if (_current_word == 0)
	{
		while (_content_file[--_current_line].empty())
			;
		_current_word = _content_file[_current_line].size();
	}
	return _content_file[_current_line][--_current_word];
}
