/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 16:17:02 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/07 14:13:13 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Channel.hpp"
#include <string>
#include <iostream>

void Server::sendToChannel(Client* sender, const std::string& target, const std::string& message) {
	Channel* channel = getChannelByName(target);
	if (!channel) {
		sendNumericReply(sender, "403", target, "No such channel");
		return;
	}
	
	if (!channel->isMember(sender)) {
		sendNumericReply(sender, "404", target, "Cannot send to channel");
		return;
	}
	std::string fullMsg = ":" + sender->getPrefix() + " PRIVMSG " + target + " :" + message;
	channel->broadcast(*this, fullMsg, sender);

}

void Server::sendToUser(Client* sender, const std::string& target, const std::string& message) {
	Client* recipient = getClientByNick(target);
	if (!recipient) {
		sendNumericReply(sender, "401", target, "No such nick/channel");
		return;
	}
	std::string fullMsg = 	":" + sender->getPrefix() + " PRIVMSG " + target + " :" + message;
	sendToClient(recipient->getFd(), fullMsg);
}

void Server::handleMsg(Client* client, const Message& msg) {
	if (!client->isRegistered()) {
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

	if (target[0] == '#')
		sendToChannel(client, target, message);
	else
		sendToUser(client, target, message);
}
