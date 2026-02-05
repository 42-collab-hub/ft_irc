/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 18:54:01 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/05 18:00:29 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"
#include "Client.hpp"
#include <string>

void Server::handleUser(Client* client, const Message& msg) {
	if (!_password.empty() && !client->isAuthenticated())
		return;
	if (client->isRegistered()) {
		sendNumericReply(client, "462", "", "You may not reregister");
		return;
	}
	if (msg._params.size() < 4) {
		sendNumericReply(client, "461", msg._command, "Not enough parameters");
		return;
	}

	std::string username = msg._params[0];
	if (username.empty())
		sendNumericReply(client, "461", msg._command, "Not enough parameters");

	if (username.size() > USERLEN)
		username = username.substr(0, USERLEN);

	std::string realname = msg._params[3];
	if (realname.empty() && !client->getNickname().empty())
		realname = client->getNickname();

	client->setUsername(username);
	client->setRealname(realname);
	checkRegistration(client);
}
