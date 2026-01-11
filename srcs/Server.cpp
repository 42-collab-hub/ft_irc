/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 15:17:48 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/11 20:41:42 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>
#include <cstring> // std::strlen
#include <sys/socket.h> // socket() setsockopt()
#include <netinet/in.h> // struct sockaddr_in
#include <unistd.h> // close
#include <cerrno> // errno
#include <poll.h> // poll()
// #include "Client.hpp"

Server::Server(int port, const std::string& password) : _port(port), _password(password) {}
Server::~Server() {}

// Setup listening socket - accepts new connections from clients
bool Server::setupServerSock(int& fd, struct sockaddr_in& address) {
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		std::cerr << "Error: Failed to create socket" << std::endl;
		return false;
	}
	// setsockopt() - optional

	// struct sockaddr_in address; // structure describing an Internet socket address
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 listen on all interfaces
	address.sin_port = htons(_port); // convert from host byte order to network byte order

	// int addrlen = sizeof(address);
	if (bind(fd, (sockaddr*)&address, sizeof(address)) < 0) { 	// bind socket + error handling
		std::cerr << "Error: failed to bind socket" << std::endl;
		close(fd);
		return false;
	}
	// address is a pointer to a struct sockaddr that contains the IP address and port number to bind the socket
	// sizeof(address) is size of the addr structure

	if (listen(fd, 3) < 0) { // set listening socket to passive mode - waits for new connections
		std::cerr << "Error: Failed to listen on socket" << std::endl;
		close(fd);
		return false;
	}
	return true;
}


void Server::run() {
	int fd;
	struct sockaddr_in address;
	if (!setupServerSock(fd, address))
		return ;
	std::cout << "Socket is ready to accept client connections..." << std::endl;

	// TO DO: ADD LISTENING PORT TO POLL LIST OF FDS

	// TO DO: Main loop:
	// while (1) {
	// 	TO DO - ADD poll() - waits for activity - holds until there's any activity
	// 	poll() returns & tells the server what is ready - new client / incoming message from client / client is ready to receive message from server
		
	// 	loop through all monitored fds {
	// 		if: listening socket? 
	// 			accept() new client connection & add to monitored fds for poll()
	// 		else if: client message
	// 			handle client message
	// 		else if: client disconnected
	// 			handle client removal
	// 	}
	// }
	
	// Accept connections from clients - single client
	int addrlen = sizeof(address);
	int	clientFd = accept(fd, (sockaddr*)&address, (socklen_t*)&addrlen);
	if (clientFd < 0) {
		std::cerr << "Error: Failed to accept connection: " << strerror(errno) << std::endl;
		return ;
	}
	std::cout << "Client connected (fd=" << clientFd << ")" << std::endl;

	// char buffer[1024] = {0};
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	// read message from client
	int readBytes = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
	if (readBytes < 0) {
		std::cerr << "Error: Failed to read from client: " << strerror(errno) << std::endl;
		close(clientFd);
		return ;
	}
	else if (readBytes == 0) {
		// Client disconnected gracefully
		std::cout << "Client disconnected" << std::endl;
		close(clientFd);
		return ;
	}
	else { // readBytes > 0
		std::cout << "Client says: " << buffer << std::endl;
	}

	// send reply to client
	const char* reply = "message from server";
	send(clientFd, reply, std::strlen(reply), 0);	

	close(fd);
	close(clientFd);
}
