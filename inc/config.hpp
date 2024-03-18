/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snaji <snaji@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 16:06:32 by snaji             #+#    #+#             */
/*   Updated: 2024/03/18 18:29:58 by snaji            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP
# include <cstddef>
# include <string>
# include <vector>
# include <map>

class	Server {
	private:
		std::string	ip;
		int			port;
		std::string	server_name;
		std::size_t	client_max_body_size;
		std::map<int, std::string>	error_pages;
		int			fd;

	public:
		void	set_ip(std::string const& ip);
		void	set_port(int const port);
		void	set_server_name(std::string const& server_name);
		void	set_client_max_body_size(std::size_t const size);
		void	add_error_page(int const error, std::string const& page);
		void	set_fd(int const fd);
		
		std::string	get_ip(void) const;
		int			get_port(void) const;
		std::string	get_server_name(void) const;
		std::size_t	get_client_max_body_size(void) const;
		std::string get_error_page(int const error) const;
		int			get_fd(void) const;
};

class	Config {
	private:
		std::vector<Server>	servers;
		
	public:
		Config(std::string const& config_file);	
		Server& get_server(int fd);
		
};

#endif