/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 18:54:01 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/18 20:25:09 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include <iostream>

void Server::checkRegistration(Client* client) {
	if (_password.empty() || !client->isAuthenticated()) // PASS is not done
		return;
	if (client->getNickname().empty()) // NICK is not done
		return;
	if (client->getUsername().empty()) // USER is not done
		return;

	client->setRegister(true); // Only after both USER & NICK
	// sendWelcome(client); // send welcome messages
	std::cout << "User registered: " << client->getUsername() << std::endl;
}

void Server::handleUser(Client* client, const Message& msg) {
	if (client->isRegistered()) { // user may not reregister ERR_ALREADYREGISTERED 462
		return (sendError(client, 462, "You may not reregister"));
	}
	if (msg._params.size() < 4) { // username must not be empty, has exactly 4 params ERR_NEEDMOREPARAMS 461
		return (sendError(client, 461, "Not enough parameters"));
	}
	
	// TODO: add username validation & truncation for max length

	client->setUsername(msg._params[0]);
	client->setRealname(msg._params[3]);

	checkRegistration(client);
}
