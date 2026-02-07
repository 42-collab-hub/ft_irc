/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 13:32:19 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/07 12:35:10 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Channel.hpp"
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

void Server::handleModeK(bool adding, Channel* channel, std::string& appliedModes, std::string& appliedParams, char& lastSign, std::string param) {
	if (adding)
		channel->setKey(param);
	else
		channel->unsetKey();

	appendAppliedMode(appliedModes, lastSign, adding, 'k');
	if (adding)
		appliedParams += " " + param;

}

void Server::handleModeL(bool adding, Channel* channel, std::string& appliedModes, std::string& appliedParams, char& lastSign, std::string param, Client* client) {
	if (adding) {
		if (!isUnsignedNumber(param)) {
			sendNumericReply(client, "461", "MODE", "Not enough parameters");
			return;
		}
		int limit = std::atoi(param.c_str());
		if (limit == 0) {
			sendNumericReply(client, "461", "MODE", "Not enough parameters");
			return;
		}

		channel->setUserLimit(limit);
		appendAppliedMode(appliedModes, lastSign, adding, 'l');
		appliedParams += " " + param;
	}
	else {
		channel->unsetUserLimit();
		appendAppliedMode(appliedModes, lastSign, adding, 'l');
	}
}

void Server::handleModeO(bool adding, Channel* channel, std::string& appliedModes, std::string& appliedParams, char& lastSign, std::string param, Client* client) {
	std::string channelName = channel->getName();
	Client* target = getClientByName(param);
	if (!target) {
		sendNumericReply(client, "401", param, "No such nick");
		return;
	}
	if (!channel->isMember(target)) {
		sendNumericReply(client, "441", param + " " + channelName, "They aren't on that channel");
		return;
	}

	bool ok = false;
	if (adding)
		ok = channel->addOperator(target);
	else
		ok = channel->removeOperator(target);

	if (ok) {
		appendAppliedMode(appliedModes, lastSign, adding, 'o');
		appliedParams += " " + param;
	}
}

void Server::setChannelModes(Channel* channel, Client* client, const Message& msg) {
	std::string modestring = msg._params[1];
	size_t paramIndex = 2;   
	bool adding = true;
	std::string appliedModes = "";
	std::string appliedParams = "";
	char lastSign = 0;

	std::string channelName = channel->getName();

	for (size_t i = 0; i < modestring.size(); ++i)
	{
		char m = modestring[i];

		if (m == '+') { adding = true; continue; }
		if (m == '-') { adding = false; continue; }

		if (m != 'i' && m != 't' && m != 'k' && m != 'l' && m != 'o') {
			std::string mm(1, m);
			sendNumericReply(client, "472", mm, "is unknown mode char to me");
			continue;
		}

		bool wantParam = needsParam(m, adding);
		std::string param;
		
		if (wantParam)
		{
			if (paramIndex >= msg._params.size())
			{
				sendNumericReply(client, "461", "MODE", "Not enough parameters");
				break;
			}
			param = msg._params[paramIndex++];
		}

		if (m == 'i' || m == 't')
		{
			channel->setMode(m, adding);
			appendAppliedMode(appliedModes, lastSign, adding, m);
		}
		else if (m == 'k')
			handleModeK(adding, channel, appliedModes, appliedParams, lastSign, param);
		else if (m == 'l')
			handleModeL(adding, channel, appliedModes, appliedParams, lastSign, param, client);
		else if (m == 'o')
			handleModeO(adding, channel, appliedModes, appliedParams, lastSign, param, client);
	}
	if (!appliedModes.empty())
	{
		std::string out = ":" + client->getPrefix() + " MODE " + channelName + " " + appliedModes + appliedParams;
		channel->broadcast(*this, out, NULL);
	}
}

void Server::viewChannelModes(Channel* channel, Client* client) {
	std::string channelName = channel->getName();
	std::string channelModes = channel->getModes();
	sendNumericReply(client, "324", channelName + " " + channelModes, "");
}

void Server::handleMode(Client* client, const Message& msg) {
	if (!client->isRegistered()) {
		sendNumericReply(client, "451", "", "You have not registered");
		return;
	}
	if (msg._params.size() < 1 || msg._params[0].empty()) {
		sendNumericReply(client, "461", msg._command, "Not enough parameters");
		return;
	}

	std::string channelName = msg._params[0];
	if (msg._params[0][0] != '#')
		return;

	Channel* chan = getChannel(channelName);
	if (!chan) {
		sendNumericReply(client, "403", channelName, "No such channel");
		return ;
	}
	if (!chan->isMember(client)) {
		sendNumericReply(client, "442", channelName, "You're not on that channel");
		return ;
	}

	if (msg._params.size() == 1 || (msg._params.size() > 1 && msg._params[1].empty())) {
		viewChannelModes(chan, client);
		return ;
	}

	if (!chan->isOperator(client)) {
		sendNumericReply(client, "482", channelName, "You're not channel operator");
		return ;
	}

	setChannelModes(chan, client, msg);
}
