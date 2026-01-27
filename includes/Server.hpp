/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:33:59 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/27 16:10:49 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#define IRC_MESSAGE_MAX_LENGTH 512
#define USERLEN 12

#include <netinet/in.h> // for holding the IP address & port - contains structure and variable definitions
#include <arpa/inet.h> // inet_pton and similar
#include <string>
#include <map>
#include "Client.hpp"
#include <poll.h>
#include "Message.hpp"

class Server {
private:
	std::string				_creationTime;
	int						_listenSocket;
	int						_port;
	std::string				_password;
	std::map<int, Client*> 	_clients;
	std::vector<pollfd> 	_poll_fds;
	// channels

	static Server*			_instance; // static pointer to the Server object
	bool					_running; // main loop flag

	void setServerCreationTime(void); 
	void handleNewConnection(void);
	void handleClientMessage(int fd);
	void removeClient(int fd);
	void handleCommand(Client* client, const Message& msg);
	void sendNumericReply(Client* client, const std::string& code, const std::string& params, const std::string& message);
	void sendToClient(int fd, const std::string& message);
	void handlePass(Client* client, const Message& msg);
	void handleCap(Client* client, const Message& msg);
	void handleNick(Client* client, const Message& msg);
	bool isTakenNickname(Client* client, const std::string& newNickname);
	void handleUser(Client* client, const Message& msg);
	void handlePing(Client* client, const Message& msg);
	void handleMsg(Client* client, const Message& msg);
	void sendToUser(Client* sender, const std::string& target, const std::string& message);
	void sendToChannel(Client* sender, const std::string& target, const std::string& message);
	void checkRegistration(Client* client);
	void sendWelcome(Client* client);
	void shutdownServer(void);

	// signal handling
	static void registerSignalHandlers(void);
	static void signalHandler(int signum);

public:
	Server(int port, const std::string& password);
	~Server();
	void run();
	bool init();
};

#endif