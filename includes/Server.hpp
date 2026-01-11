/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:33:59 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/11 20:40:55 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <netinet/in.h> // for holding the IP address & port - contains structure and variable definitions
#include <arpa/inet.h> // inet_pton and similar
#include <string>
#include <map>
#include "Client.hpp"

class Server {
private:
	// int						_fd;
	int						_port;
	std::string				_password;
	// std::map<int, Client*> 	_clients;
	// std::vector<pollfd> 	poll_fds;
	// channels

	bool setupServerSock(int& fd, struct sockaddr_in& address);

public:
	Server(int port, const std::string& password);
	~Server();
	
	void run();
};

#endif