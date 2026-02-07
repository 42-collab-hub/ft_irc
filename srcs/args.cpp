/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 17:40:10 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/07 13:58:02 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "args.hpp"
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cerrno>

bool validPort(const char* str, int& port) {
	if (str == NULL || str[0] == '\0')
		return false;

	size_t len = std::strlen(str);
	if (std::isspace(str[0]) || std::isspace(str[len - 1]))
		return false;

	char* endptr;
	errno = 0;
	long value = std::strtol(str, &endptr, 10);
	if (*endptr != '\0' || errno == ERANGE)
		return false;

	if (value < 1 || value > 65535)
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
