/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:31:03 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/10 17:50:05 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "args.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	int port;
	std::string password;

	if (!validArgs(argc, argv, port, password))
		return 1;
	std::cout << "arguments are valid, can proceed" << std::endl;

	// create server
		// start server
		// configure connection of client to server
		// listen for client requests
		// reply to client

	// Create Server
	// Server server(port, password);
	
	return 0;
}