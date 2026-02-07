/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 15:17:48 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/07 12:51:22 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include <iostream>
#include <string>
#include <cstring> // memset
#include <sys/socket.h> // socket() setsockopt()
#include <netinet/in.h> // struct sockaddr_in
#include <unistd.h> // close
#include <cerrno> // errno
#include <poll.h> // poll()
#include <signal.h> // signal()
#include <ctime>
#include <fcntl.h>
#include <exception>

Server* Server::_instance = NULL;

Server::Server(int port, const std::string& password) {
	_port = port;
	_password = password;
	_listenSocket = -1;
	_serverName = "ircserv";
	_instance = this;
	_running = true;
	setServerCreationTime();
}

void Server::setServerCreationTime() {
	time_t timestamp;
	struct tm datetime;
	char output[50];

	time(&timestamp);
	datetime = *localtime(&timestamp);
	std::strftime(output, sizeof(output), "%a %b %d %Y %H:%M:%S", &datetime);
	_creationTime = output;
}

Server::~Server() {
	if (_listenSocket >= 0)
		close(_listenSocket);

	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		close(it->first);
		delete it->second;
	}

	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete *it;

	_clients.clear();
	_channels.clear();
	_poll_fds.clear();
}

void Server::init() {
	createServerSocket();
	bindSocket();

	if (listen(_listenSocket, 10) < 0) { // set listening socket to passive mode - waits for new connections
		close(_listenSocket);
		throw std::runtime_error(std::string("Failed to listen: ") + strerror(errno));
	}

	pollfd listenPollFd = {_listenSocket, POLLIN, 0};
	_poll_fds.push_back(listenPollFd);
}


void Server::createServerSocket(void) {
	_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenSocket < 0)
		throw std::runtime_error(std::string("Failed to create socket: ") + strerror(errno));

	fcntl(_listenSocket, F_SETFL, O_NONBLOCK); // set server socket to non-blocking mode

	int opt = 1;
	if (setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		close(_listenSocket);
		throw std::runtime_error(std::string("Failed to set socket options: ") + strerror(errno));
	}
}

void Server::bindSocket(void) {
	struct sockaddr_in address; // structure describing an Internet socket address
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 listen on all interfaces
	address.sin_port = htons(_port); // convert from host byte order to network byte order

	if (bind(_listenSocket, (sockaddr*)&address, sizeof(address)) < 0) { // bind socket
		close(_listenSocket);
		throw std::runtime_error(std::string("Failed to bind socket: ") + strerror(errno));
	}
	// address is a pointer to a struct sockaddr that contains the IP address and port number to bind the socket
	// sizeof(address) is size of the addr structure
}

void Server::run() {
	registerSignalHandlers();

	while (_running) {
		int ready = poll(_poll_fds.data(), _poll_fds.size(), -1);
		if (ready < 0)
			break;

		for (size_t i = 0; i < _poll_fds.size(); i++) { // check each monitored fd at a time
			int fd = _poll_fds[i].fd;
			short revents = _poll_fds[i].revents;
			Client* client = getClientByFd(fd);

			if (revents == 0) // no activity
				continue;
			if (revents & (POLLHUP | POLLERR)) { // errors/disconnections
				if (fd != _listenSocket)
					removeClient(fd);
				continue;
			}
			if (revents & POLLIN) { // incoming client activity
				if (fd == _listenSocket)
					handleNewConnection();
				else
					handleClientMessage(fd);
			}
			client = getClientByFd(fd); // re-fetch client
			if (!client) // check if client disconnected
				continue;
			if (revents & POLLOUT) { // client ready to receive
				client->flushMessage(); // send message
				if (!client->hasQueuedMessage())
					disablePollout(fd);
			}
			if (client->hasQueuedMessage())
				enablePollout(fd);
		}
	}
	shutdownServer();
}

void Server::shutdownServer() {
	std::string message = "Shutting down IRC server...";
	std::cout << "\n" + message << std::endl;
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		int fd = it->first;
		sendToClient(fd, "ERROR :" + message + "\r\n");
		it->second->flushMessage();
	}
}

void Server::handleNewConnection(void) {
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	int	clientFd = accept(_listenSocket, (sockaddr*)&address, (socklen_t*)&addrlen);
	if (clientFd < 0) {
		std::cerr << "Error: Failed to accept connection: " << strerror(errno) << std::endl;
		return ;
	}
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	std::string hostname = inet_ntoa(address.sin_addr);
	Client* newClient = new Client(clientFd, hostname);
	_clients[clientFd] = newClient;
	pollfd clientPollFd = {clientFd, POLLIN, 0};
	_poll_fds.push_back(clientPollFd);
}

void Server::destroyChannel(Channel* channel) {
	if (!channel)
		return ;

	std::string channelName = channel->getName();
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (*it == channel)
		{
			delete *it;
			_channels.erase(it);
			std::cout << "destroyChannel destroyed channel : " << channelName << std::endl;
			return ;
		}
	}
}

void Server::queueToClient(Client* c, const std::string& msg)
{
	if (!c)
		return ;
	sendToClient(c->getFd(), msg);
}

void Server::handleClientMessage(int fd) {
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	ssize_t readBytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (readBytes <= 0) // reading error / client disconnected
		return (removeClient(fd));
	buffer[readBytes] = '\0';

	Client* client = getClientByFd(fd);
	if (!client) {
		std::cerr << "Error: Client not found for fd " << fd << std::endl;
		return;
	}
	client->appendToBuffer(buffer, readBytes);

	while (client->hasCompleteMessage()) {
		std::string raw = client->extractMessage(); // extract single message
		std::cout  << "Processing: " << raw << std::endl;

		if (raw.size() > IRC_MESSAGE_MAX_LENGTH) // Message is too long ERR_INPUTTOOLONG 417
			sendNumericReply(client, "417", "", "Input line was too long");
		else {
			Message msg;
			msg.parse(raw); // parse single message
			handleCommand(client, msg); // execute single command

			client = getClientByFd(fd); // re-fetch pointer
			if (!client) {
				std::cout << "Client removed, stopping message processing" << std::endl;
				return;
			}
		}
	}
}

void Server::removeClient(int fd) { // client removal upon disconnection
	std::map<int, Client*>::iterator it = _clients.find(fd); // delete Client object
	if (it != _clients.end()) {
		delete it->second;
		_clients.erase(it);
	}
	for (size_t i = 0; i < _poll_fds.size(); i++) { // remove Client fd from pollfd vector
		if (_poll_fds[i].fd == fd) {
			_poll_fds.erase(_poll_fds.begin() + i);
			break;
		}
	}
	close(fd);
	std::cout << "Client " << fd << " disconnected and successfully removed" << std::endl;
}

void Server::registerSignalHandlers(void) {
    signal(SIGINT, Server::signalHandler);
    signal(SIGTERM, Server::signalHandler);
    signal(SIGQUIT, Server::signalHandler);
}

void Server::signalHandler(int signum) {
	(void)signum;
    if (_instance)
		_instance->_running = false;
}

void Server::sendToClient(int fd, const std::string& message) {
	std::string msg = message + "\r\n";
	Client *c = getClientByFd(fd);
	c->queueMessage(msg);
	enablePollout(fd);
}

void Server::enablePollout(int fd) {
	for (size_t i = 0; i < _poll_fds.size(); ++i) {
		if (_poll_fds[i].fd == fd) { 
			_poll_fds[i].events |= POLLOUT;
			return;
		}
	}
}

void Server::disablePollout(int fd) {
	for (size_t i = 0; i < _poll_fds.size(); ++i) {
		if (_poll_fds[i].fd == fd) { 
			_poll_fds[i].events &= ~POLLOUT;
			return;
		}
	}
}

Channel* Server::getChannelByName(const std::string& name) {
	for (size_t i = 0; i < _channels.size(); i++) {
		if (_channels[i]->getName() == name)
			return _channels[i];
	}
	return NULL;	
}

Client*	Server::getClientByNick(const std::string& nick) {
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second->getNickname() == nick)
			return it->second;
	}
	return NULL;
}

Client* Server::getClientByFd(int fd) {
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return NULL;
	return it->second;
}

Client* Server::getClientByName(std::string& name)
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it).second->getNickname() == name)
			return (*it).second;
	}
	return NULL;
}

void Server::sendNumericReply(Client* client, const std::string& code, const std::string& params, const std::string& message) {
	std::string target = client->getNickname().empty() ? "*" : client->getNickname();
	std::string reply = ":" + _serverName + " " + code + " " + target;

	if (!params.empty())
		reply += " " + params;
	if (!message.empty())
		reply += " :" + message;
	sendToClient(client->getFd(), reply);
}

void Server::handleCommand(Client* client, const Message& msg) {
	std::string cmd = msg._command;

	if (cmd == "PASS")
		handlePass(client, msg);
	else if (cmd == "CAP")
		handleCap(client, msg);
	else if (cmd == "INVITE")
		handleInvite(client, msg);
	else if (cmd == "JOIN")
		handleJoin(client, msg);
	else if (cmd == "KICK")
		handleKick(client, msg);
	else if (cmd == "MODE")
		handleMode(client, msg);
	else if (cmd == "NICK")
		handleNick(client, msg);
	else if (cmd == "PART")
		handlePart(client, msg);
	else if (cmd == "PING")
		handlePing(client, msg);
	else if (cmd == "PRIVMSG")
		handleMsg(client, msg);
	else if (cmd == "QUIT")
		handleQuit(client, msg);
	else if (cmd == "TOPIC")
		handleTopic(client, msg);
	else if (cmd == "USER")
		handleUser(client, msg);
	else if (cmd == "WHOIS")
		sendNumericReply(client, "318", client->getNickname(), "End of /WHOIS list");
	else
		sendNumericReply(client, "421", cmd, "Unknown command");
}
