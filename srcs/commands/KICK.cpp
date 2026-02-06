/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/01 06:01:02 by gholloco          #+#    #+#             */
/*   Updated: 2026/02/06 16:10:03 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <iostream>

void Server::handleKick(Client *client, const Message &msg) {
	if (!client->isRegistered()) {
		sendNumericReply(client, "451", "", "You have not registered");
		return;
	}

	if (msg._params.size() < 2 || msg._params[0].empty() || msg._params[1].empty()) {
		sendNumericReply(client, "461", msg._command, "Not enough parameters");
		return;
	}

	std::string channelName = msg._params[0];
	Channel *chan = getChannel(channelName);
	if (!chan) {
		sendNumericReply(client, "403", channelName, "No such channel");
		return;
	}

	if (!chan->isMember(client)) {
		sendNumericReply(client, "442", channelName, "You're not on that channel");
		return;
	}

	if (!chan->isOperator(client)) {
		sendNumericReply(client, "482", channelName, "You're not channel operator");
		return;
	}

	std::string kickedName = msg._params[1];
	Client *kickedClient = getClientByName(kickedName);
	if (!kickedClient) {
		sendNumericReply(client, "401", kickedName, "No such nick");
		return;
	}

	if (!chan->isMember(kickedClient)) {
		sendNumericReply(client, "441", kickedName + " " + channelName, "They aren't on that channel");
		return;
	}

	std::string reason = (msg._params.size() > 2 && !msg._params[2].empty()) ? msg._params[2] : "Kicked by operator";
	std::string kickMessage = ":" + client->getPrefix() + " KICK " + channelName + " " + kickedName + " :" + reason;
	chan->broadcast(*this, kickMessage, NULL);
	chan->removeMember(kickedClient);
}