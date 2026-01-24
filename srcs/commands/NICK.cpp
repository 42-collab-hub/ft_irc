/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 20:40:43 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/24 21:14:04 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include <cctype> // isalnum, isalpha
#include <string>

static bool validNickname(const std::string& str) { // validate nickname character set
	if (str.empty() || str.length() > 9)
		return false;

	std::string set = "[]{}\\|`_^";

	if (!std::isalpha(str[0]) && set.find(str[0]) == std::string::npos) // first character
		return false;

	for (size_t i = 1; i < str.length(); i++) {
		if (!std::isalnum(str[i]) && str[i] != '-' && set.find(str[i]) == std::string::npos)
			return false;
	}
	return true;
}

static std::string normalizeNickname(const std::string& original) {
	std::string normalized = "";
	for (size_t i = 0; i < original.length(); i++) {
		normalized += std::tolower(original[i]);
	}
	return normalized;
}

void Server::handleNick(Client* client, const Message& msg) {
	if (!_password.empty() && !client->isAuthenticated()) // NICK + USER only after PASS
		return;
	if (msg._params.empty() || msg._params[0].empty()) { // empty parameter ERR_NONICKNAMEGIVEN (431)
		sendToClient(client->getFd(), ":server 431 * :No nickname given");
		return;
	}

	std::string newNickname = msg._params[0];
	std::string currentNick = client->getNickname();
	if (!currentNick.empty() && (normalizeNickname(currentNick) == normalizeNickname(newNickname)))
		return;

	if (!validNickname(newNickname)) { // invalid character set ERR_ERRONEUSNICKNAME (432)
		sendToClient(client->getFd(), ":server 432 " + newNickname + " :Erroneus nickname");
		return;
	}

	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second == client) // skip itself
			continue;
		std::string otherNickname = it->second->getNickname();
		if (normalizeNickname(otherNickname) == normalizeNickname(newNickname)) {
			sendToClient(client->getFd(), ":server 433 " + newNickname + " :Nickname is already in use");
			return;
		}
	}

	client->setNickname(newNickname);
	std::string user = client->getUsername();
	std::string host = client->getHostname();
	if (currentNick.empty())
		currentNick = "*";
	sendToClient(client->getFd(), ":" + currentNick + "!" + user + "@" + host + " NICK :" + newNickname);
}
