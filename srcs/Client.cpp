/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 10:45:13 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/17 12:35:37 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd) : _fd(fd), _nickname(""), _username(""), _hostname(""), _recvBuffer("") {}
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
