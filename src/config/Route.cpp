/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snaji <snaji@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 01:09:42 by snaji             #+#    #+#             */
/*   Updated: 2024/03/20 16:18:27 by snaji            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Route.hpp"
#include <stdexcept>
#include <string>

static bool	next_word(std::string::iterator& it, std::string::iterator end) {
	while (it != end && *it != ' ' && *it != '\t' && *it != '\n')
		++it;
	while (it != end && (*it == ' ' || *it == '\t' || *it == '\n'))
		++it;
	return (it != end);
}

static std::string get_word(std::string::iterator it, std::string::iterator end) {
	std::string	word;
	for (; it != end && *it != ' ' && *it != '\t' && *it != '\n' && *it != ';'; ++it)
		word.push_back(*it);
	return (word);
}

static std::string get_next_word(std::string::iterator& it, std::string::iterator end) {
	if (!next_word(it, end))
		return ("");
	return (get_word(it, end));
}

Route::Route(std::string route) {
	std::vector<std::string> tokens;
	tokens.push_back("method");
	tokens.push_back("redirect");
	tokens.push_back("directory");
	tokens.push_back("cgi");
	tokens.push_back("rewrite");
	
	std::string::iterator it = route.begin();
	
	if (route.compare(0, 5, "route") != 0 || !next_word(it, route.end()))
		throw std::invalid_argument("invalid argument in route creation");
	route_path = get_word(it, route.end());
	if (!next_word(it, route.end()) || get_word(it, route.end()) != "{")
		throw std::invalid_argument("invalid argument in route creation");

	method = ALL;
	while (next_word(it, route.end())) {
		std::string word = get_word(it, route.end());
		if (word == "method") {
			
		}
	}
}