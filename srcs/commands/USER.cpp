/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 18:54:01 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/26 18:33:26 by gholloco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"
#include "Client.hpp"
#include <iostream>

void Server::handleUser(Client* client, const Message& msg) {
	if (client->isRegistered()) { // user may not reregister ERR_ALREADYREGISTERED 462
		sendNumericReply(client, "462", "", "You may not reregister");
		return;
	}
	if (msg._params.size() < 4) { // has 4 params ERR_NEEDMOREPARAMS 461
		sendNumericReply(client, "461", msg._command, "Not enough parameters");
		return;
	}

	std::string username = msg._params[0];
	if (username.empty()) // username must not be empty ERR_NEEDMOREPARAMS 461
		sendNumericReply(client, "461", msg._command, "Not enough parameters");

	if (username.size() > USERLEN) // username too long, truncate to USERLEN 12
		username = username.substr(0, USERLEN);

	std::string realname = msg._params[3];
	if (realname.empty() && !client->getNickname().empty()) // use nickname as fallback to realname
		realname = client->getNickname();

	client->setUsername(username);
	client->setRealname(realname);
	checkRegistration(client);
}
