/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QUIT.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 17:39:57 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/02 19:04:55 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Channel.hpp"
#include <vector>

void Server::handleQuit(Client* client, const Message& msg) {
	std::string reason = "Client Quit";
	if (msg._params.size() > 0 && !msg._params[0].empty())
		reason = msg._params[0];
	std::string message = ":" + client->getPrefix() + " QUIT :" + reason;

	// send message to all channel the client was member of
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		if ((*it)->isMember(client)) {
			(*it)->broadcast(*this, message, client);
			(*it)->removeMember(client);
		}
	}
	sendToClient(client->getFd(), "ERROR :Closing Link: " + reason); // send message to Client
	removeClient(client->getFd()); // close connection to server
}
