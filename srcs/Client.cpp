/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 10:45:13 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/25 16:53:27 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd, const std::string& hostname) : _fd(fd), _nickname(""), _username(""), _hostname(hostname), _recvBuffer(""), _autheticated(false), _registered(false) {}
Client::~Client() {}

void Client::appendToBuffer(const char* data, size_t len) {
	_recvBuffer.append(data, len);
}

bool Client::hasCompleteMessage() const {
	return _recvBuffer.find("\r\n") != std::string::npos;
}

std::string Client::extractMessage() {
	size_t pos = _recvBuffer.find("\r\n");
	if (pos == std::string::npos)
		return "";
	std::string message = _recvBuffer.substr(0, pos); // extract message without \r\n
	_recvBuffer.erase(0, pos + 2); // remove from buffer including \r\n
	return message;
}

void Client::setAuthenticated(bool status) { _autheticated = status; }
void Client::setRegister(bool status) { _registered = status; }
void Client::setNickname(const std::string& str) { _nickname = str; }
void Client::setUsername(const std::string& str) { _username = str; }
void Client::setRealname(const std::string& str) { _realname = str; }
void Client::setHostname(const std::string& str) { _hostname = str; }

bool Client::isAuthenticated(void) { return _autheticated; }
bool Client::isRegistered(void) { return _registered; }
const std::string& Client::getNickname(void) { return _nickname; }
const std::string& Client::getUsername(void) { return _username; } 
const std::string& Client::getRealname(void) { return _realname; }
const std::string& Client::getHostname(void) { return _hostname; }
int Client::getFd(void) { return this->_fd; }
