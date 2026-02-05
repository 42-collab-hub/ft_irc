/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PASS.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 12:32:06 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/05 17:49:59 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"

void Server::handlePass(Client* client, const Message& msg) {
	if (client->isRegistered()) {
		sendNumericReply(client, "462", "", "You may not reregister");
		return;
	}
	if (msg._params.empty()) {
		sendNumericReply(client, "461", msg._command, "Not enough parameters");
		return;
	}
	if (msg._params[0] != _password) {
		sendNumericReply(client, "464", "", "Password incorrect");
		return;
	}
	client->setAuthenticated(true);
}