/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 13:32:19 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/06 21:28:19 by gholloco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include <cstdlib>

static bool needsParam(char m, bool adding)
{
	// o always needs a nick
	if (m == 'o')
		return true;
	// k/l only need a param when adding
	if ((m == 'k' || m == 'l') && adding)
		return true;
	return false;
}

static bool isUnsignedNumber(const std::string& s)
{
	if (s.empty())
		return false;
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (s[i] < '0' || s[i] > '9')
			return false;
	}
	return true;
}

static void appendAppliedMode(std::string& appliedModes, char& lastSign, bool adding, char mode)
{
	char sign = adding ? '+' : '-';
	if (appliedModes.empty() || lastSign != sign)
	{
		appliedModes += sign;
		lastSign = sign;
	}
	appliedModes += mode;
}

void Server::handleMode(Client* client, const Message& msg) {
	if (!client->isRegistered()) {
		sendNumericReply(client, "451", "", "You have not registered");
		return;
	}
	if (msg._params.empty() || msg._params[0].empty() || msg._params[0][0] != '#') // ignore all non-channel mode commands
		return;

	std::string channelName = msg._params[0];
	Channel* chan = getChannel(channelName);

	// TODO:: delete these debug prints
	// std::cout << ""

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
	if (msg._params.size() == 1) // requesting MODES, doesn't need to be an operator
	{
		sendToClient(client->getFd(), ":server 324 " + client->getNickname() + " " + channelName + " " + chan->getModes());
		return ;
	}
	if (!chan->isOperator(client))
	{
		sendNumericReply(client, "482", channelName, ":You're not channel operator");
		return ;
	}
	if (msg._params[1].empty()) 
	{ 
		sendNumericReply(client, "461", msg._command, "Not enough parameters");
		return ;
	}

	const std::string& modes = msg._params[1];
	size_t paramIndex = 2;   
	bool adding = true;
	std::string appliedModes;
	std::string appliedParams;
	char lastSign = 0;

	for (size_t i = 0; i < modes.size(); ++i)
	{
		char m = modes[i];

		if (m == '+') { adding = true; continue; }
		if (m == '-') { adding = false; continue; }

		if (m != 'i' && m != 't' && m != 'k' && m != 'l' && m != 'o')
		{
			std::string mm(1, m);
			sendNumericReply(client, "472", mm, ":is unknown mode char to me");
			continue;
		}

		bool wantParam = needsParam(m, adding);
		std::string param;

		if (wantParam)
		{
			if (paramIndex >= msg._params.size())
			{
				sendNumericReply(client, "461", "MODE", ":Not enough parameters");
				break;
			}
			param = msg._params[paramIndex++];
		}

		if (m == 'i' || m == 't')
		{
			chan->setMode(m, adding);
			appendAppliedMode(appliedModes, lastSign, adding, m);
		}
		else if (m == 'k')
		{
			if (adding)
				chan->setKey(param);
			else
				chan->unsetKey();

			appendAppliedMode(appliedModes, lastSign, adding, 'k');
			if (adding)
				appliedParams += " " + param;
		}
		else if (m == 'l')
		{
			if (adding)
			{
				if (!isUnsignedNumber(param))
				{
					sendNumericReply(client, "461", "MODE", ":Not enough parameters");
					break;
				}
				int limit = std::atoi(param.c_str());
				if (limit == 0)
				{
					sendNumericReply(client, "461", "MODE", ":Not enough parameters");
					break;
				}

				chan->setUserLimit(limit);
				appendAppliedMode(appliedModes, lastSign, adding, 'l');
				appliedParams += " " + param;
			}
			else
			{
				chan->unsetUserLimit();
				appendAppliedMode(appliedModes, lastSign, adding, 'l');
			}
		}
		else if (m == 'o')
		{
			Client* target = getClientByName(param);
			if (!target)
			{
				sendNumericReply(client, "401", param, ":No such nick");
				continue;
			}
			if (!chan->isMember(target))
			{
				sendNumericReply(client, "441", param + " " + channelName, ":They aren't on that channel");
				continue;
			}

			bool ok = false;
			if (adding)
				ok = chan->addOperator(target);
			else
				ok = chan->removeOperator(target);

			if (ok)
			{
				appendAppliedMode(appliedModes, lastSign, adding, 'o');
				appliedParams += " " + param;
			}
		}
	}
	if (!appliedModes.empty())
	{
		std::string out = ":" + client->getPrefix() + " MODE " + channelName + " " + appliedModes + appliedParams;
		chan->broadcast(*this, out, NULL);
	}
}
