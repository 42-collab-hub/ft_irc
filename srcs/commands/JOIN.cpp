/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gholloco <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 13:19:11 by gholloco          #+#    #+#             */
/*   Updated: 2026/01/24 18:39:21 by gholloco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"
#include "Message.hpp"
#include "Channel.hpp"

int Server::channelExists(const std::string &name)
{
	for (std::vector<Channel*>::iterator chan = _channels.begin(); chan != _channels.end(); chan++)
	{
		if ((*chan)->getName() == name)
		{
			return 1;
		}
	}
	std::cout << "No such chan : " << name << std::endl;
	return 0;
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

void Server::handleJoin(Client* client, const Message& msg)
{
	std::string clientName = client->getNickname();
	if (!client->isAuthenticated() || !client->isRegistered())
		return;
	if (msg._params.empty() || msg._params[0].empty())
	{
		sendToClient(client->getFd(), ":server 461 " + clientName + msg._command + ":Not enough parameters");
		return ;
	}
	std::string channelName = msg._params[0];
	std::cout << "channelName : " << channelName << std::endl;
	if (!validChannelName(channelName))
	{
		sendToClient(client->getFd(), ":server 476 " + clientName + " " + channelName + ":Bad Channel Mask");
		return ;
	}
	if (channelExists(channelName))
	{
		// check chan password & chan invites
	}
	else
	{
		// create new channel, name is already validated
		Channel newChan(channelName);
		_channels.push_back(&newChan);
		newChan.addMember(client);
		newChan.addOperator(client);
		sendToClient(client->getFd(), ":" + clientName + " JOIN " + channelName);
		sendToClient(client->getFd(), ":server 331 " + clientName + " " + channelName + " :No topic is set");
		sendToClient(client->getFd(), ":server 353 " + clientName + " = " + channelName + " :@" + clientName);
		sendToClient(client->getFd(), ":server 366 " + clientName + " " + channelName + " :End of /NAMES list");
		std::cout << "Sucessfully created the channel : " << channelName << std::endl;
	}
	

}
