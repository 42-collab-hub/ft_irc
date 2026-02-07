/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 13:19:11 by gholloco          #+#    #+#             */
/*   Updated: 2026/02/07 12:55:20 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"
#include "Message.hpp"
#include "Channel.hpp"
#include "Client.hpp"

Channel* Server::getChannel(const std::string &name) {
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		if ((*it)->getName() == name) {
			return *it;
		}
	}
	return NULL;
}

static bool validChannelName(const std::string &name) {
	if (name.empty() || name[0] != '#')
		return false;
	for (size_t i = 0; i < name.length(); i++) {
		if (name[i] == ' ' || name[i] == ',' || name[i] == '\a')	
			return false;
	}
	return true;
}

void Server::sendJoinMessage(Client* client, Channel* channel) {
	std::string channelName = channel->getName();
	std::string joinMessage = ":" + client->getPrefix() + " JOIN " + channelName;
	channel->broadcast(*this, joinMessage, NULL);
	sendTopicInfo(client, channel);
	sendNumericReply(client, "353", "= " + channelName, channel->getMemberList());
	sendNumericReply(client, "366", channelName, "End of /NAMES list");
}

void Server::handleJoin(Client* client, const Message& msg) {
	if (!client->isRegistered()) {
		sendNumericReply(client, "451", "", "You have not registered");
		return;
	}

	if (msg._params.size() < 1) {
		sendNumericReply(client, "461", msg._command, "Not enough parameters");
		return ;
	}

	std::string channelName = msg._params[0];
	std::string channelKey = (msg._params.size() >= 2) ? msg._params[1] : "";

	if (!validChannelName(channelName)) {
		sendNumericReply(client, "476", channelName, "Bad Channel Mask");
		return ;
	}

	Channel* chan = getChannel(channelName);

	if (chan) {
		// TODO: I think this numeric reply is only for INVITE, unsure
		// if (chan->isMember(client)) {
		// 	sendNumericReply(client, "443", client->getNickname(), chan->getName() + " is already on channel");
		// 	return;
		// }
		if (chan->isInviteOnly() && !chan->isInvited(client)) {
			sendNumericReply(client, "473", chan->getName(), "Cannot join channel (+i)");
			return;
		}
		if (chan->hasKey() && channelKey != chan->getPassword()) {
			sendNumericReply(client, "475", chan->getName(), "Cannot join channel (+k)");
			return;
		}
		if (chan->hasUserLimit() && chan->getUserLimit() == chan->getMemberCount()) {
			sendNumericReply(client, "471", chan->getName(), "Cannot join channel (+l)");
			return;
		}
		chan->addMember(client);
		chan->removeInvite(client);
	}
	else {
		chan = new Channel(channelName);
		_channels.push_back(chan);
		chan->addMember(client);
		chan->addOperator(client);
	}
	sendJoinMessage(client, chan);
}
