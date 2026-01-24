/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 16:05:26 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/24 21:03:12 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include <string>
#include <sstream>

Message::Message() {}
Message::~Message() {}

void Message::parse(const std::string& raw) {
	if (raw.empty())
		return;
	std::string str = raw;
	size_t pos = 0;

	if (str[0] == ':') { // extract prefix if present
		pos = str.find(' ');
		_prefix = str.substr(1, pos - 1); // skip the ':'
		str = str.substr(pos + 1);
	}

	pos = str.find(' '); // extract command
	if (pos == std::string::npos) { // no parameters
		_command = str;
		return;
	}
	_command = str.substr(0, pos);
	str = str.substr(pos + 1);

	while (!str.empty()) { // extract parameters
		if (str[0] == ':') { // trailing parameter (rest of string)
			_params.push_back(str.substr(1));
			break;
		}
		pos = str.find(' ');
		if (pos == std::string::npos) { // last parameter
			_params.push_back(str);
			break;
		}
		_params.push_back(str.substr(0, pos));
		str = str.substr(pos + 1);
	}
}