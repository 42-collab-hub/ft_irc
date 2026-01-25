/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CAP.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 20:37:31 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/25 16:51:59 by mglikenf         ###   ########.fr       */
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
	std::string nick = client->getNickname();
	sendNumericReply(client, "001", "", "Welcome to the IRC network " + client->getNickname());
	sendNumericReply(client, "002", "", "Your host is " + client->getHostname() + ", running version 1.0");
	sendNumericReply(client, "003", "", "This server was created " + _creationTime);
	sendNumericReply(client, "004", "", "Channel modes iktol");
}