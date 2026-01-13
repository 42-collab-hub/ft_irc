/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 08:32:00 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/13 22:32:26 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <string>
#include <vector>

// CRLF="\r\n"

// message format & numeric replies

class Message {
public:
	std::string 				prefix_;
	std::string					command_;
	std::vector<std::string>	params;

	void parse(const std::string& raw);
};

#endif