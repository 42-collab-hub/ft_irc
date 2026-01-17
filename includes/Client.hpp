/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 08:38:00 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/17 12:35:50 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <vector>
#include <string>

class Client {
private:
	int						_fd;
	std::string				_nickname;
	std::string				_username;
	std::string				_hostname;
	// bool					_autheticated; // client authentification state
	// bool					_registered; // client registration state
	std::string				_recvBuffer; // receive buffer (for partial messages)
	// channels
	
public:
	Client(int fd);
	~Client();

	void appendToBuffer(const char* data, size_t len);
	bool hasCompleteMessage() const;
	std::string extractMessage();
};

#endif