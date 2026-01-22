/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:31:03 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/19 12:00:30 by mglikenf         ###   ########.fr       */
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

	Server server(port, password);
	if (!server.init()) {
		return 1;
	}
	server.run();

	return 0;
}
