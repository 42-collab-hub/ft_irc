/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:33:59 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/18 20:24:05 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <netinet/in.h> // for holding the IP address & port - contains structure and variable definitions
#include <arpa/inet.h> // inet_pton and similar
#include <string>
#include <map>
#include "Client.hpp"
#include <poll.h>
#include "Message.hpp"

#define USERLEN 12

class Server {
private:
	int						_listenSocket;
	int						_port;
	std::string				_password;
	std::map<int, Client*> 	_clients;
	std::vector<pollfd> 	_poll_fds;
	// channels

	bool setupServerSocket(void);
	void handleNewConnection(void);
	void handleClientMessage(int fd);
	void removeClient(int fd);
	
	void handleCommand(Client* client, const Message& msg);
	void sendToClient(int fd, const std::string& message);
	void sendError(Client* client, int code, const std::string& message);
	void handlePass(Client* client, const Message& msg);
	// void handleCap(Client* client, const Message& msg);
	// void handleNick(Client* client, const Message& msg);
	void handleUser(Client* client, const Message& msg);
	void checkRegistration(Client* client);
	// void sendWelcome(Client* client);

public:
	Server(int port, const std::string& password);
	~Server();
	void run();
};

#endif