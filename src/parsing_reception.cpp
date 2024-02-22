/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_reception.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wmari <wmari@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 13:15:51 by wmari             #+#    #+#             */
/*   Updated: 2024/02/22 14:03:37 by wmari            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"
#include "logger.hpp"

int	parsing_reception(int client_sock, logger log, std::string req)
{
	if (req.find("GET") != std::string::npos)
	{
		std::ifstream file;
		std::string filename;
		filename = "www/" + req.substr(5, req.find_first_of("HTTP/1.1", 5));
		file.open(filename.c_str());
		if (!file.is_open())
		{
			log.log(ERROR, "file open error: " + std::string(strerror(errno)));
			return -1;
		}
		std::string reponse;
		std::string line;

		while (std::getline(file, line))
			reponse += line + "\n";
		file.close();
		reponse = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + reponse;
		log.log(INFO, "response: \n" + reponse);
		send(client_sock, reponse.c_str(), reponse.size(), 0);
		return (reponse.size());
	}
	else
		return (-1);
}