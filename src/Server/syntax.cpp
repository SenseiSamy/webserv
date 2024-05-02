#include "Server.hpp"
#include <iostream>

int Server::syntax_brackets()
{
    size_t open_brackets = 0;
    size_t close_brackets = 0;
    std::string last_word;

    for (size_t line = 0; line < _file_config_content.size(); ++line)
    {
        for (size_t i = 0; i < _file_config_content[line].size(); ++i)
        {
            // last word
            if (i > 0)
                last_word = _file_config_content[line][i - 1];
            else if (line > 0)
                last_word = _file_config_content[line - 1].back();
            std::string word = _file_config_content[line][i];
            if (word == "{")
            {
                ++open_brackets;
                if (last_word != "server" && last_word != "route")
                    return (std::cerr << "Syntax error: Unexpected '{' at line " << line + 1 << "." << std::endl, -1);
            }
            else if (word == "}")
                ++close_brackets;
        }
    }

    if (open_brackets != close_brackets)
        return (std::cerr << "Syntax error: Unbalanced brackets." << std::endl, -1);

    return 0;
}
