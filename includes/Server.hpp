/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:33:59 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/07 12:56:52 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#define IRC_MESSAGE_MAX_LENGTH 510
#define USERLEN 12
#define NICKLEN 9

#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include <netinet/in.h> // for holding the IP address & port - contains structure and variable definitions
#include <arpa/inet.h> // inet_pton and similar
#include <string>
#include <vector>
#include <map>
#include <poll.h>

class Server {
private:
	std::string				_serverName;
	std::string				_creationTime;
	int						_listenSocket;
	int						_port;
	std::string				_password;

	std::map<int, Client*> 	_clients;
	std::vector<pollfd> 	_poll_fds;
	std::vector<Channel*>	_channels;

	static Server*			_instance; // static pointer to the Server object
	bool					_running; // main loop flag

	Server(const Server& src);
	Server& operator=(const Server& other);

	void createServerSocket(void);
	void bindSocket(void);

	void setServerCreationTime(void);
	void handleNewConnection(void);
	void checkRegistration(Client* client);
	void sendWelcome(Client* client);
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
	void handlePart(Client* client, const Message& msg);
	void handleTopic(Client* client, const Message& msg);
	void sendTopicInfo(Client* client, Channel* channel);
	void handleInvite(Client* client, const Message& msg);
	void handleKick(Client* client, const Message& msg);
	void handleMode(Client* client, const Message& msg);
	void viewChannelModes(Channel* channel, Client* client);
	void handleModeK(bool adding, Channel* channel, std::string& appliedModes, std::string& appliedParams, char& lastSign, std::string param);
	void handleModeL(bool adding, Channel* channel, std::string& appliedModes, std::string& appliedParams, char& lastSign, std::string param, Client* client);
	void handleModeO(bool adding, Channel* channel, std::string& appliedModes, std::string& appliedParams, char& lastSign, std::string param, Client* client);
	void handleQuit(Client* client, const Message& msg);
	void setChannelModes(Channel* channel, Client* client, const Message& msg);
	void destroyChannel(Channel* channel);
	void handleJoin(Client* client, const Message& msg);
	void sendJoinMessage(Client* client, Channel* channel);

	void shutdownServer(void);
	
	// signal handling
	static void registerSignalHandlers(void);
	static void signalHandler(int signum);

	Client* 	getClientByFd(int fd);
	Client*		getClientByNick(const std::string& nick);
	Channel*	getChannelByName(const std::string& name); // double
	Client* 	getClientByName(std::string& name);
	Channel* 	getChannel(const std::string& name);

	void 		enablePollout(int fd);
	void 		disablePollout(int fd);

public:
	Server(int port, const std::string& password);
	~Server();
	void init();
	void run();
	void queueToClient(Client* c, const std::string& msg);
};

#endif
