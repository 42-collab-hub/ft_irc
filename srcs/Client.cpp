/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 10:45:13 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/29 08:00:58 by gholloco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd, const std::string& hostname) : _fd(fd), _nickname(""), _username(""), _hostname(hostname), _recvBuffer(""), _autheticated(false), _registered(false) {}
Client::~Client() {}

void Client::appendToBuffer(const char* data, size_t len) {
	_recvBuffer.append(data, len);
}

void Client::queueMessage(const std::string& msg)
{
	_sendBuffer += msg;
}

void Client::flushMessage(void)
{
	if (_sendBuffer.empty())
		return ;

	ssize_t i = send(_fd, _sendBuffer.c_str(), _sendBuffer.size(), 0);

	if (i <= 0) // message not sent so we don't erase it yet, retry on next POLLOUT
		return ; 

	_sendBuffer.erase(0, i);
}

bool Client::hasQueuedMessage() const
{
	return !_sendBuffer.empty();
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

std::string Client::getPrefix() 
{
	return _nickname + "!" + _username + "@" + _hostname;
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
