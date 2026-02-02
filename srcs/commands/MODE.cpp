/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 13:32:19 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/30 14:08:54 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"

void Server::handleMode(Client* client, const Message& msg) {
	// TODO - add isRegistered check
	if (msg._params.empty())
		return;
	if (msg._params[0][0] != '#') // ignore all non-channel mode commands
		return;

	// TODO: handle channel modes
	(void)client;
	(void)msg;
	
}