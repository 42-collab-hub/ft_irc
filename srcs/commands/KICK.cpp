/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gholloco <gwendal.hollocou@orange.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/01 06:01:02 by gholloco          #+#    #+#             */
/*   Updated: 2026/02/03 02:24:24 by gholloco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include <iostream>

void Server::handleKick(Client *client, const Message &msg) {
  std::string reason;
  std::string clientName = client->getNickname();

  if (!client->isAuthenticated() || !client->isRegistered())
    return;
  if (msg._params.size() <= 1 || msg._params[0].empty() ||
      msg._params[1].empty()) {
    sendToClient(client->getFd(), ":server 461 " + clientName + " " +
                                      msg._command + " :Not enough parameters");
    return;
  }
  if (msg._params.size() >= 3)
    reason = msg._params[2];
  std::string channelName = msg._params[0];
  std::string kickedName = msg._params[1];
  std::cout << "invitedName = " << kickedName << std::endl;
  std::cout << "channelName : " << channelName << std::endl;
  Channel *chan = getChannel(channelName);
  Client *kickedClient = getClientByName(kickedName);
  if (!chan) {
    sendNumericReply(client, "403", channelName, ":No such channel");
    return;
  }
  if (!chan->isMember(client)) {
    sendNumericReply(client, "442", channelName, ":You're not on that channel");
    return;
  }
  if (!chan->isOperator(client)) {
    sendNumericReply(client, "482", channelName,
                     ":You're not channel operator");
    return;
  }
  if (!kickedClient) {
    sendNumericReply(client, "401", kickedName, ":No such nick");
    return;
  }
  if (kickedClient && !chan->isMember(kickedClient)) {
    sendToClient(client->getFd(), ":server 441 " + clientName + " " +
                                      kickedName + " " + channelName +
                                      " :They aren't on that channel");
    return;
  }
  chan->broadcast(*this,
                  ":" + client->getPrefix() + " KICK " + channelName + " " +
                      kickedName + " :" + reason,
                  NULL);
  chan->removeMember(kickedClient);
}
