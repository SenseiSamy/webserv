/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snaji <snaji@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 18:08:25 by snaji             #+#    #+#             */
/*   Updated: 2024/03/18 18:26:13 by snaji            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"
#include <exception>
#include <string>

void	Server::set_ip(std::string const &ip) {
	this->ip = ip;
}

void	Server::set_port(int const port) {
	this->port = port;
}

void	Server::set_server_name(std::string const& server_name) {
	this->server_name = server_name;
}

void	Server::set_client_max_body_size(std::size_t const size) {
	client_max_body_size = size;
}

void	Server::add_error_page(int const error, std::string const& page) {
	error_pages[error] = page;
}	

void	Server::set_fd(int const fd) {
	this->fd = fd;
}

std::string	Server::get_ip(void) const {
	return (ip);	
}

int	Server::get_port(void) const {
	return (port);
}

std::string	Server::get_server_name(void) const {
	return (server_name);
}

std::size_t	Server::get_client_max_body_size(void) const {
	return (client_max_body_size);
}

std::string Server::get_error_page(int const error) const {
	try {
		return (error_pages.at(error));
	}
	catch (std::exception &e) {
		return (std::string(""));
	}
}

int	Server::get_fd(void) const {
	return (fd);
}
