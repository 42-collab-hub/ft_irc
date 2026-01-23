/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gholloco <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 13:19:11 by gholloco          #+#    #+#             */
/*   Updated: 2026/01/23 18:26:51 by gholloco         ###   ########.fr       */
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
	std::cout << "No such chan." << std::endl;
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
	std::string channelName = msg._params[0];
	std::cout << "channelName : " << channelName << std::endl;
	if (!client->isAuthenticated() || !client->isRegistered())
		return;
	if (msg._params.empty() || channelName.empty())
	{
		sendToClient(client->getFd(), ":server 461 " + client->getNickname() + msg._command + ":Not enough parameters");
		return ;
	}
	if (validChannelName(channelName))
	{
		sendToClient(client->getFd(), ":server 476 " + client->getNickname() + channelName + ":Bad Channel Mask");
		return ;
	}
	if (channelExists(channelName))
	{
		// check chan password & chan invites
	}
	else
	{
		// create channel with the channelName
	}
	

}
