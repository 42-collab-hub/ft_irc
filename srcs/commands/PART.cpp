/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PART.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gholloco <gwendal.hollocou@orange.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 18:51:34 by gholloco          #+#    #+#             */
/*   Updated: 2026/02/02 21:29:13 by gholloco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"
#include "Message.hpp"
#include "Channel.hpp"

void Server::handlePart(Client* client, const Message& msg)
{
	std::string clientName = client->getNickname();
	std::string reason;

	if (msg._params.size() >= 2)
		reason = msg._params[1];
	if (!client->isAuthenticated() || !client->isRegistered())
		return ;
	if (msg._params.empty() || msg._params[0].empty())
	{
		sendToClient(client->getFd(), ":server 461 " + clientName + " " + msg._command + " :Not enough parameters");
		return ;
	}
	std::string channelName = msg._params[0];
	std::cout << "channelName : " << channelName << std::endl;
	std::cout << "reason = " << reason << std::endl;

	Channel* chan = getChannel(channelName);
	if (!chan)
	{
		sendNumericReply(client, "403", channelName, ":No such channel");
		return ;
	}
	if (chan && !chan->isMember(client))
	{
		sendNumericReply(client, "442", chan->getName(), ":You're not on that channel");
		return ;
	}
	std::string partMessage = ":" + client->getPrefix() + " PART " + channelName;
	if (!reason.empty())
		partMessage += " :" + reason;
	chan->broadcast(*this, partMessage, NULL);
	chan->removeMember(client);
	if (chan->getMemberCount() == 0)
		destroyChannel(chan);
}
