/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PART.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 18:51:34 by gholloco          #+#    #+#             */
/*   Updated: 2026/02/07 14:00:28 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"
#include "Message.hpp"
#include "Channel.hpp"
#include "Client.hpp"

void Server::handlePart(Client* client, const Message& msg) {
	if (!client->isRegistered()) {
		sendNumericReply(client, "451", "", "You have not registered");
		return;
	}
	if (msg._params.empty() || msg._params[0].empty()) {
		sendNumericReply(client, "461", msg._command, "Not enough parameters");
		return;
	}

	std::string channelName = msg._params[0];
	Channel* chan = getChannel(channelName);
	if (!chan) {
		sendNumericReply(client, "403", channelName, "No such channel");
		return ;
	}

	if (!chan->isMember(client)) {
		sendNumericReply(client, "442", chan->getName(), "You're not on that channel");
		return ;
	}

	std::string reason = (msg._params.size() > 1 && !msg._params[1].empty()) ? msg._params[1] : "Left channel";
	std::string partMessage = ":" + client->getPrefix() + " PART " + channelName + " :" + reason;
	chan->broadcast(*this, partMessage, NULL);
	chan->removeMember(client);
	if (chan->getMemberCount() == 0)
		destroyChannel(chan);
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
			std::cout << "Successfully removed empty channel " << channelName << std::endl;
			return ;
		}
	}
}