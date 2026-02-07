/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:31:03 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/07 13:15:20 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "args.hpp"
#include <string>
#include <exception>
#include <iostream>

int main(int argc, char **argv)
{
	int port;
	std::string password;

	if (!validArgs(argc, argv, port, password))
		return 1;

	try {
		Server server(port, password);
		std::cout << "Starting IRC server..." << std::endl;
		server.init();
		std::cout << "Running IRC server on port " << port << std::endl;
		server.run();

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
