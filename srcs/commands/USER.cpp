/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 18:54:01 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/23 15:46:09 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include <iostream>

void Server::sendWelcome(Client* client) {
	sendToClient(client->getFd(), ":server 001 " + client->getNickname() + " :Welcome to the IRC network " + client->getNickname()); // 001 RPL_WELCOME
	sendToClient(client->getFd(), ":server 002 " + client->getNickname() + " :Your host is " + client->getHostname() + ", running version 1.0"); // 002 RPL_YOURHOST
	sendToClient(client->getFd(), ":server 003 " + client->getNickname() + " :This server was created today"); // 003 RPL_CREATED
	sendToClient(client->getFd(), ":server 004 " + client->getNickname() + " :Channel modes iktol"); // 004 RPL_MYINFO
}

void Server::checkRegistration(Client* client) {
	if (!_password.empty() && !client->isAuthenticated()) // PASS is not done
		return;
	if (client->getNickname().empty()) // NICK is not done
		return;
	if (client->getUsername().empty()) // USER is not done
		return;

	client->setRegister(true); // Only after both USER & NICK
	sendWelcome(client); // send welcome messages
	// displayClientInfo(client); // debug function
}

void Server::handleUser(Client* client, const Message& msg) {
	if (client->isRegistered()) { // user may not reregister ERR_ALREADYREGISTERED 462
		return (sendError(client, 462, "You may not reregister"));
	}
	if (msg._params.size() < 4) { // has 4 params ERR_NEEDMOREPARAMS 461
		return (sendError(client, 461, "Not enough parameters"));
	}

	std::string username = msg._params[0];
	if (username.empty()) { // username must not be empty ERR_NEEDMOREPARAMS 461
		return (sendError(client, 461, "Not enough parameters"));
	}
	if (username.size() > USERLEN) { // username too long, truncate to USERLEN 12
		username = username.substr(0, USERLEN);
	}

	std::string realname = msg._params[3];
	if (realname.empty() && !client->getNickname().empty()) { // use nickname as fallback to
		realname = client->getNickname();
	}
	client->setUsername(username);
	client->setRealname(realname);
	checkRegistration(client);
}
