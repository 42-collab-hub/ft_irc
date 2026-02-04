/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 17:40:10 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/31 13:48:32 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "args.hpp"
#include <iostream>
#include <cstdlib> // std::strtol
#include <cctype> // std::isspace
#include <cstring> // std::strlen
#include <cerrno> // errno

bool validPort(const char* str, int& port) {
	if (str == NULL || str[0] == '\0')
		return false;

	size_t len = std::strlen(str); // strict input, no leading/trailing spaces
	if (std::isspace(str[0]) || std::isspace(str[len - 1]))
		return false;

	char* endptr;
	errno = 0;
	long value = std::strtol(str, &endptr, 10); // convert to numeric value
	if (*endptr != '\0' || errno == ERANGE)
		return false;

	if (value < 1 || value > 65535) // valid port range 1-65535
		return false;
	port = static_cast<int>(value);
	return true;
}

bool isWhitespace(const std::string& str) {
    for (size_t i = 0; i < str.length(); ++i) {
        if (!std::isspace(str[i]))
            return false;
    }
    return true;
}

bool validPassword(const char* str, std::string& password) {
	if (str == NULL || str[0] == '\0')
		return false;
	if (isWhitespace(str))
		return false;
	password = str;
	return true;
}

bool validArgs(int argc, char **argv, int& port, std::string& password) {
	if (argc != 3) {
		std::cerr << "Usage format: ./ircserv <port> <password>" << std::endl;
		return false;
	}
	if (!validPort(argv[1], port)) {
		std::cerr << "Invalid port input" << std::endl;
		return false;		
	}
	if (!validPassword(argv[2], password)) {
		std::cerr << "Invalid password input" << std::endl;
		return false;
	}
	return true;
}
