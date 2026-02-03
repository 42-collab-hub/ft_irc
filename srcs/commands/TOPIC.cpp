/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gholloco <gwendal.hollocou@orange.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 20:18:13 by gholloco          #+#    #+#             */
/*   Updated: 2026/01/31 20:12:07 by gholloco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>

void Server::handleTopic(Client *client, const Message& msg)
{
	std::string clientName = client->getNickname();
	std::string newTopic;

	if (msg._params.size() >= 2)
		newTopic = msg._params[1];
	if (!client->isAuthenticated() || !client->isRegistered())
		return;
	if (msg._params.empty() || msg._params[0].empty())
	{
		sendToClient(client->getFd(), ":server 461 " + clientName + msg._command + ":Not enough parameters");
		return ;
	}
	std::string channelName = msg._params[0];
	std::cout << "channelName : " << channelName << std::endl;
	std::cout << "topic = " << newTopic << std::endl;
	Channel* chan = getChannel(channelName);
	if (!chan)
	{
		sendNumericReply(client, "403", channelName, ":No such channel");
		return ;
	}
	if (chan && !chan->isMember(client))
	{
		sendNumericReply(client, "442", channelName, ":You're not on that channel");
		return ;
	}
	if (msg._params.size() == 1)
	{
		if (chan->getTopic().empty())
			sendNumericReply(client, "331", channelName, ":No topic is set");
		else
			sendNumericReply(client, "332", channelName, ":" + chan->getTopic());
		return ;
	}
	if (chan->isTopicRestricted() && !chan->isOperator(client))
	{
		sendNumericReply(client, "482", channelName, ":You're not channel operator");
        return ;
	}
	chan->setTopic(newTopic);
	chan->broadcast(*this, ":" + client->getPrefix() + " TOPIC " + channelName + " :" + newTopic, NULL);
}
