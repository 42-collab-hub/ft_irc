/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CAP.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 20:37:31 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/03 19:10:38 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"

void Server::handleCap(Client* client, const Message& msg) {
	if (msg._params.empty())
		return;
	
	std::string subcmd = msg._params[0];

	if (subcmd == "LS")
		sendToClient(client->getFd(), ":server CAP * LS :");
	else if (subcmd == "END")
		checkRegistration(client);
}

void Server::checkRegistration(Client* client) {
	if (client->isRegistered()) // already registered
		return;
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
	sendNumericReply(client, "001", "", "Welcome to the IRC network " + client->getPrefix());
	sendNumericReply(client, "002", "", "Your host is " + _serverName + ", running version 1.0");
	sendNumericReply(client, "003", "", "This server was created " + _creationTime);
	sendNumericReply(client, "004", client->getNickname(), _serverName + " 1.0 - iktol");
	sendNumericReply(client, "005", "NICKLEN=9 USERLEN=12", "are supported by this server");
}