/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 15:17:48 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/17 12:32:02 by mglikenf         ###   ########.fr       */
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
#include <algorithm> // std::find

Server::Server(int port, const std::string& password) : _port(port), _password(password) {
}

Server::~Server() {
	close(_listenSocket);
	for (size_t i = 0; i < _poll_fds.size(); i++)
		close(_poll_fds[i].fd);
}

// Setup listening socket - accepts new connections from clients
bool Server::setupServerSocket(void) {
	// struct sockaddr_in address; // structure describing an Internet socket address
	struct sockaddr_in address;
	// int addrlen = sizeof(address);

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

	// int addrlen = sizeof(address);
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
	Client* newClient = new Client(clientFd);
	_clients[clientFd] = newClient;
	pollfd clientPollFd = {clientFd, POLLIN, 0};
	_poll_fds.push_back(clientPollFd);
}

void Server::handleClientMessage(int fd) {
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	// TODO: handle blocking of recv with fcntl
	ssize_t readBytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (readBytes < 0) { // reading error
		return (removeClient(fd));
		std::cerr << "Error: Failed to read from client: " << strerror(errno) << std::endl;
	}
	if (readBytes == 0)
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
		// execute single command
	}

	// TODO: Parse & handle IRC commands
	// parse message
	// execute one command - executeCommand(_clients[fd], message);
}

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

void Server::run() {
	if (!setupServerSocket())
		return;

	while (true) {
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
}


// ________________________

