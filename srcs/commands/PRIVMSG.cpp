/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 16:17:02 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/27 17:53:23 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include <string>
#include <iostream>

void Server::sendToChannel(Client* sender, const std::string& target, const std::string& message) {
	(void)sender;
	(void)target;
	(void)message;
}

void Server::sendToUser(Client* sender, const std::string& target, const std::string& message) {
	// check if such client exists
	std::map<int, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second->getNickname() == target)
			break;
	}
	if (it == _clients.end()) { // no client can be found for the supplied nickname
		sendNumericReply(sender, "401", target, "No such nick/channel");
		return;
	}
	std::string fullMsg = 	":" + sender->getNickname() + "!" +
							sender->getUsername() + "@" +
							sender->getHostname() + " PRIVMSG " +
							target + " :" + message;
	sendToClient(it->second->getFd(), fullMsg);
}


void Server::handleMsg(Client* client, const Message& msg) {
	if (!client->isRegistered()) { 	// sender must be registered
		sendNumericReply(client, "451", "", "You have not registered");
		return;
	}
	if (msg._params.size() < 1 || msg._params[0].empty()) {
		sendNumericReply(client, "411", "", "No recipient given (" + msg._command + ")");
		return;
	}
	if (msg._params.size() < 2 || msg._params[1].empty()) {
		sendNumericReply(client, "412", "", "No text to send");
		return;
	}

	std::string target = msg._params[0];
	std::string message = msg._params[1];

	if (target[0] == '#') // target is channel
		sendToChannel(client, target, message);
	else // target is user
		sendToUser(client, target, message);
}

// ERR_NOSUCHNICK (401) - no client can be found for the supplied nickname
// ERR_CANNOTSENDTOCHAN (404) - message cannot be delivered to a channel
// ERR_NORECIPIENT (411) - message wasn’t delivered because there was no recipient given
// ERR_NOTEXTTOSEND (412) - message wasn’t delivered because there was no text to send