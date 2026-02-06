/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 18:28:22 by gholloco          #+#    #+#             */
/*   Updated: 2026/02/06 14:39:41 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Channel.hpp"

void Server::handleInvite(Client* client, const Message& msg) {
	if (!client->isRegistered()) {
		sendNumericReply(client, "451", "", "You have not registered");
		return;
	}

	if (msg._params.size() < 2 || msg._params[0].empty() || msg._params[1].empty()) {
		sendNumericReply(client, "461", msg._command, "Not enough parameters");
		return ;
	}

	std::string invitedName = msg._params[0];
	Client* invitedClient = getClientByName(invitedName);
	if (!invitedClient) {
		sendNumericReply(client, "401", invitedName, "No such nick");
		return ;
	}

	std::string channelName = msg._params[1];
	Channel* chan = getChannel(channelName);
	if (!chan) {
		sendNumericReply(client, "403", channelName, "No such channel");
		return ;
	}

	if (!chan->isMember(client)) {
		sendNumericReply(client, "442", channelName, "You're not on that channel");
		return ;
	}

	if (!chan->isOperator(client)) {
		sendNumericReply(client, "482", channelName, "You're not channel operator");
		return ;
	}

	if (chan->isMember(invitedClient)) {
		std::string params = invitedName + " " + channelName;
		sendNumericReply(client, "443", params, "is already on channel");
		return ;
	}

	chan->invite(invitedClient);
	sendNumericReply(client, "341", invitedName + " " + channelName,  "");
	sendToClient(invitedClient->getFd(), ":" + client->getPrefix() + " INVITE " + invitedName + " " + channelName);
}
