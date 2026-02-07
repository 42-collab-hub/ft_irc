/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 08:32:00 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/07 14:30:06 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <string>
#include <vector>

class Message {
public:
	std::string 				_prefix;
	std::string					_command;
	std::vector<std::string>	_params;

	Message();
	~Message();

	void parse(const std::string& raw);
};

#endif