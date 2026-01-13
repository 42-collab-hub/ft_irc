/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 08:38:00 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/13 22:18:00 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <vector>
#include <string>

class Client {
private:
	int						_fd;
	std::string				_nickname;
	std::string				_username;
	std::string				_hostname;
	// int						_auth; // authentification state
	// receive buffer (for partial messages)
	// channels
	
public:
	Client(int fd);
	~Client();
};

#endif