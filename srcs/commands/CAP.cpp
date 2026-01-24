/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CAP.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 20:37:31 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/24 21:00:15 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"

void Server::handleCap(Client* client, const Message& msg) {
	if (msg._params.empty())
		return;
	
	std::string subcmd = msg._params[0];

	if (subcmd == "LS") {
		sendToClient(client->getFd(), ":server CAP * LS :");
	}
	else if (subcmd == "END") {
		checkRegistration(client);
	}
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
}

void Server::sendWelcome(Client* client) {
	std::string nick = client->getNickname();
	sendToClient(client->getFd(), ":server 001 " + nick + " :Welcome to the IRC network " + client->getNickname()); // 001 RPL_WELCOME
	sendToClient(client->getFd(), ":server 002 " + nick + " :Your host is " + client->getHostname() + ", running version 1.0"); // 002 RPL_YOURHOST
	sendToClient(client->getFd(), ":server 003 " + nick + " :This server was created " + _creationTime); // 003 RPL_CREATED
	sendToClient(client->getFd(), ":server 004 " + nick + " :Channel modes iktol"); // 004 RPL_MYINFO
}
