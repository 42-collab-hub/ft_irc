/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 15:17:48 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/20 12:40:36 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"
#include "Client.hpp"
#include <iostream>
#include <cstring> // std::strlen
#include <sys/socket.h> // socket() setsockopt()
#include <netinet/in.h> // struct sockaddr_in
#include <unistd.h> // close
#include <cerrno> // errno
#include <poll.h> // poll()
#include <sstream> // std::ostringstream
#include <signal.h> // signal()
#include <stdlib.h> // exit()

Server* Server::_instance = NULL;

Server::Server(int port, const std::string& password) : _port(port), _password(password) {
	_instance = this;
	_running = true;
}
// initialize the other variables as well

Server::~Server() {
	close(_listenSocket);
    // Close all client sockets and free memory
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        close(it->first);
        delete it->second;
    }
    _clients.clear();
	_poll_fds.clear();
}

bool Server::init() {
	// struct sockaddr_in address; // structure describing an Internet socket address
	struct sockaddr_in address;

	_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenSocket < 0) {
		std::cerr << "Error: Failed to create socket" << std::endl;
		return false;
	}
	int opt = 1;
	if (setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "Error: Failed to set socket options: " << strerror(errno) << std::endl;
		close(_listenSocket);
		return false;
	}
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 listen on all interfaces
	address.sin_port = htons(_port); // convert from host byte order to network byte order

	if (bind(_listenSocket, (sockaddr*)&address, sizeof(address)) < 0) { 	// bind socket + error handling
		std::cerr << "Error: failed to bind socket: " << strerror(errno) << std::endl;
		close(_listenSocket);
		return false;
	}
	// address is a pointer to a struct sockaddr that contains the IP address and port number to bind the socket
	// sizeof(address) is size of the addr structure

	if (listen(_listenSocket, 10) < 0) { // set listening socket to passive mode - waits for new connections
		std::cerr << "Error: Failed to listen: " << strerror(errno) << std::endl;
		close(_listenSocket);
		return false;
	}
	pollfd listenPollFd = {_listenSocket, POLLIN, 0};
	_poll_fds.push_back(listenPollFd);
	std::cout << "Socket is ready to accept client connections..." << std::endl;

	return true;
}

void Server::handleNewConnection(void) {
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	int	clientFd = accept(_listenSocket, (sockaddr*)&address, (socklen_t*)&addrlen);
	if (clientFd < 0) {
		std::cerr << "Error: Failed to accept connection: " << strerror(errno) << std::endl;
		return ;
	}
	std::string hostname = inet_ntoa(address.sin_addr);
	Client* newClient = new Client(clientFd, hostname);
	_clients[clientFd] = newClient;
	pollfd clientPollFd = {clientFd, POLLIN, 0};
	_poll_fds.push_back(clientPollFd);
}

void Server::handleClientMessage(int fd) {
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	// TODO: handle blocking of recv with fcntl
	ssize_t readBytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (readBytes <= 0) // reading error / client disconnected
		return (removeClient(fd));
	buffer[readBytes] = '\0';

	// TODO: handle max length limit -> truncate to 512 / send error message / ignore

	Client* client = _clients[fd];
	client->appendToBuffer(buffer, readBytes);

	while (client->hasCompleteMessage()) {
		std::string raw = client->extractMessage(); // extract single message
		std::cout  << "Processing: " << raw << std::endl;
		Message msg;
		msg.parse(raw); // parse single message
		
		handleCommand(client, msg); // execute single command

		if (_clients.find(fd) == _clients.end()) { // Client was disconnected due to error - stop processing
			std::cout << "Client removed, stopping message processing" << std::endl;
			return;
		}
		client = _clients[fd];
	}
}

void Server::run() {
    registerSignalHandlers(); // Register signal handlers

	while (_running) {
		int ready = poll(_poll_fds.data(), _poll_fds.size(), -1);
		if (ready < 0) // poll error
			break;

		for (size_t i = 0; i < _poll_fds.size(); i++) { // check each monitored fd at a time
			int fd = _poll_fds[i].fd;
			short revents = _poll_fds[i].revents;

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
				else {
					handleClientMessage(fd);
					continue;
				}
			}
		}
	}
	shutdownServer();
}

void Server::shutdownServer() {
	// send shutdown message to clients
	std::string message = "ERROR :Server shutting down\r\n";
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		int fd = it->first;
		send(fd, message.c_str(), message.length(), 0);
	}
}

// client removal upon disconnection
void Server::removeClient(int fd) {
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
	std::cout << "\nSignal detected: ";
    switch(signum) {
        case SIGINT:
			std::cout << " (SIGINT - Ctrl+C)\n"; break;
        case SIGTERM:
			std::cout << " (SIGTERM)\n"; break;
        case SIGQUIT:
			std::cout << " (SIGQUIT - Ctrl+\\)\n"; break;
        default:
			std::cout << " (Unknown)\n"; break;
    }
    if (_instance)
		_instance->_running = false;
}

// Command Handling
void Server::sendToClient(int fd, const std::string& message) {
	std::string msg = message + "\r\n";
	send(fd, msg.c_str(), msg.length(), 0);
}

void Server::sendError(Client* client, int code, const std::string& message) {
	std::string target;

	if (client->getNickname().empty())
		target = "*";
	else
		target = client->getNickname();
	
	std::ostringstream oss;
	oss << ":server " << code << " " << target << " :" << message;
	sendToClient(client->getFd(), oss.str());
}

void Server::handleCommand(Client* client, const Message& msg) {
	std::string cmd = msg._command;

	if (cmd == "PASS")
		handlePass(client, msg);
	// else if (cmd == "CAP")
	// 	handleCap(client, msg);
	else if (cmd == "NICK")
		handleNick(client, msg);
	else if (cmd == "USER")
		handleUser(client, msg);
	else
		std::cout << "Command not implemented: " << cmd << std::endl;
}
