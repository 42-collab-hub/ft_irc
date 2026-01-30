/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gholloco <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 13:19:11 by gholloco          #+#    #+#             */
/*   Updated: 2026/01/29 20:04:49 by gholloco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"
#include "Message.hpp"
#include "Channel.hpp"

Channel* Server::getChannel(const std::string &name)
{
	std::cout << "channelExists start, every channels are : " << std::endl;
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
	  std::cout << (*it)->getName() << std::endl;
		if ((*it)->getName() == name)
		{
			return *it;
		}
	}
	std::cout << "No such chan : " << name << std::endl;
	return NULL;
}

static int validChannelName(const std::string &name)
{
	if (name.empty() || name[0] != '#')
		return 0;
	for (size_t i = 0; i < name.length(); i++)
	{
		if (name[i] == ' ' || name[i] == ',' || name[i] == '\a')	
			return 0;
	}
	return 1;
}

void Server::sendJoinMessage(Client* client, Channel* channel)
{
	std::string clientName = client->getNickname();
	std::string channelName = channel->getName();
	channel->broadcast(*this, ":" + client->getPrefix() + " JOIN " + channelName, NULL);
	sendToClient(client->getFd(), ":server 331 " + clientName + " " + channelName + " :No topic is set");
	sendToClient(client->getFd(), ":server 353 " + clientName + " = " + channelName + " :@" + channel->getMemberList());
	sendToClient(client->getFd(), ":server 366 " + clientName + " " + channelName + " :End of /NAMES list");
}

void Server::handleJoin(Client* client, const Message& msg)
{
	std::string clientName = client->getNickname();
	std::string key;

	if (msg._params.size() >= 2)
		key = msg._params[1];
	if (!client->isAuthenticated() || !client->isRegistered())
		return;
	if (msg._params.empty() || msg._params[0].empty())
	{
		sendToClient(client->getFd(), ":server 461 " + clientName + msg._command + ":Not enough parameters");
		return ;
	}
	std::string channelName = msg._params[0];
	std::cout << "channelName : " << channelName << std::endl;
	std::cout << "key = " << key << std::endl;
	if (!validChannelName(channelName))
	{
		sendToClient(client->getFd(), ":server 476 " + clientName + " " + channelName + ":Bad Channel Mask");
		return ;
	}
	Channel* chan = getChannel(channelName);
	if (chan)
	{
		if (chan->isMember(client)) // client is already member of this chan
			sendNumericReply(client, "443", chan->getName(), ":is already on channel");
		if (chan->isInviteOnly() && !chan->isInvited(client))
			sendNumericReply(client, "473", "", ":Cannot join channel (+i)");
		if (chan->hasKey() && key != chan->getPassword())
			sendNumericReply(client, "475", "", ":Cannot join channel (+k)");
		if (chan->hasUserLimit() && chan->getUserLimit() == chan->getMemberCount())
			sendNumericReply(client, "471", "", ":Cannot join channel (+l)");
		chan->addMember(client);
		chan->removeInvite(client);
		sendJoinMessage(client, chan);

	}
	else
	{
		// create new channel, name is already validated
		Channel* newChan = new Channel(channelName);
		_channels.push_back(newChan);
		newChan->addMember(client);
		newChan->addOperator(client);
		sendJoinMessage(client, newChan);
		// sendToClient(client->getFd(), ":" + client->getPrefix() + " JOIN " + channelName);
		// sendToClient(client->getFd(), ":server 331 " + clientName + " " + channelName + " :No topic is set");
		// sendToClient(client->getFd(), ":server 353 " + clientName + " = " + channelName + " :@" + newChan->getMemberList());
		// sendToClient(client->getFd(), ":server 366 " + clientName + " " + channelName + " :End of /NAMES list");
		std::cout << "Sucessfully created the channel : " << channelName << std::endl;
	}
	

}
