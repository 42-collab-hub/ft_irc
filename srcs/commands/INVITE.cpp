/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gholloco <gwendal.hollocou@orange.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 18:28:22 by gholloco          #+#    #+#             */
/*   Updated: 2026/01/31 04:37:07 by gholloco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"
#include "Message.hpp"
#include "Channel.hpp"

void Server::handleInvite(Client* client, const Message& msg)
{
	std::string clientName = client->getNickname();

	if (!client->isAuthenticated() || !client->isRegistered())
		return;
	if (msg._params.size() != 2 || msg._params[0].empty() || msg._params[1].empty())
	{
		sendToClient(client->getFd(), ":server 461 " + clientName + msg._command + ":Not enough parameters");
		return ;
	}
	std::string invitedName = msg._params[0];
	std::string channelName = msg._params[1];
	std::cout << "invitedName = " << invitedName << std::endl;
	std::cout << "channelName : " << channelName << std::endl;
	Channel* chan = getChannel(channelName);
	Client* invitedClient = getClientByName(invitedName);
	if (!invitedClient)
	{
		sendNumericReply(client, "401", invitedName, ":No such nick");
		return ;
	}
	if (!chan)
	{
		sendNumericReply(client, "403", channelName, ":No such channel");
		return ;
	}
	if (!chan->isMember(client))
	{
		sendNumericReply(client, "442", channelName, ":You're not on that channel");
		return ;
	}
	if (chan->isInviteOnly() && !chan->isOperator(client))
	{
		sendNumericReply(client, "482", channelName, ":You're not channel operator");
		return ;
	}
	if (invitedClient && chan->isMember(invitedClient))
	{
		// TODO: Not enough parameters i believe
		sendNumericReply(client, "443", channelName, ":is already on channel");
		return ;
	}
	chan->invite(invitedClient);
	sendToClient(invitedClient->getFd(), ":" + client->getPrefix() + " INVITE " + invitedName + " :" + channelName);
	sendToClient(client->getFd(), ":server 341 " + clientName + " " + invitedName + " " + channelName);
}
