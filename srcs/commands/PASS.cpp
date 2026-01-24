/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PASS.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 12:32:06 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/24 21:00:48 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"

void Server::handlePass(Client* client, const Message& msg) {
	if (client->isRegistered()) { // client is already registered ERR_ALREADYREGISTERED 462
		return (sendError(client, 462, "You may not reregister"));
	}
	if (msg._params.empty()) { // no password provided ERR_NEEDMOREPARAMS 461
		return (sendError(client, 461, "Not enough parameters"));
	}
	if (msg._params[0] != _password) { // check if passwords match ERR_PASSWDMISMATCH 464
		return (sendError(client, 464, "Password incorrect"), removeClient(client->getFd()));
	}
	client->setAuthenticated(true);
}