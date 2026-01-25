/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PING.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 11:14:49 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/25 16:52:06 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include <iostream>

void Server::handlePing(Client* client, const Message& msg) { // reply to PING by sending PONG
	if (!client->isRegistered() && !client->isAuthenticated()) // clients shouldn't PING during connection registration
		return;
	std::string token;
	if (msg._params.empty())
		token = "";
	else
		token = msg._params[0];
	sendToClient(client->getFd(), "PONG server :" + token);
}
