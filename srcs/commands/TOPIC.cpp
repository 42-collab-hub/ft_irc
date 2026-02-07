/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 20:18:13 by gholloco          #+#    #+#             */
/*   Updated: 2026/02/07 14:21:31 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include <sstream>
#include <ctime>

void Server::sendTopicInfo(Client* client, Channel* channel) {
	std::string channelName = channel->getName();

	if (channel->getTopic().empty() && channel->getTopicSetter().empty())
		sendNumericReply(client, "331", channelName, "No topic is set");
	else {
		sendNumericReply(client, "332", channel->getName(), channel->getTopic());
		if (!channel->getTopicSetter().empty()) {
			std::ostringstream oss;
			oss << channel->getTopicTime();
			std::string params = channel->getName() + " " + channel->getTopicSetter() + " " + oss.str();
			sendNumericReply(client, "333", params, "");
		}		
	}
}

void Server::handleTopic(Client *client, const Message& msg) {
	if (!client->isRegistered()) {
		sendNumericReply(client, "451", "", "You have not registered");
		return;
	}

	if (msg._params.empty() || msg._params[0].empty()) {
		sendNumericReply(client, "461", msg._command, "Not enough parameters");
		return ;
	}

	std::string channelName = msg._params[0];
	Channel* chan = getChannel(channelName);
	if (!chan) {
		sendNumericReply(client, "403", channelName, "No such channel");
		return ;
	}

	if (!chan->isMember(client)) {
		sendNumericReply(client, "442", channelName, "You're not on that channel");
		return ;
	}

	if (msg._params.size() == 1) {
		sendTopicInfo(client, chan);
		return ;
	}

	std::string newTopic = msg._params[1];
	if (chan->isTopicRestricted() && !chan->isOperator(client)) {
		sendNumericReply(client, "482", channelName, "You're not channel operator");
        return ;
	}
	chan->setTopic(newTopic, client->getNickname());
	chan->broadcast(*this, ":" + client->getPrefix() + " TOPIC " + channelName + " :" + newTopic, NULL);
}
