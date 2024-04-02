/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snaji <snaji@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 23:17:38 by snaji             #+#    #+#             */
/*   Updated: 2024/03/20 01:53:11 by snaji            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
# define ROUTE_HPP
# include <string>
# include <vector>

enum	Method {
	GET,
	POST,
	ALL
};

class	Route {
	public:
		std::string	route_path;
		struct options {
			bool	redirect;
			bool	directory;
			bool	cgi;
			bool	rewrite;
		};
		Method		method;
		std::string	redirect;
		std::string	directory;
		std::string	rewrite;
		std::vector<std::string[2]>	cgi;
		
		Route(std::string route);
};

#endif