/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 08:38:00 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/18 18:29:17 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <vector>
#include <string>

class Client {
private:
	int					_fd;
	std::string			_nickname;
	std::string			_username;
	std::string			_hostname;
	std::string			_recvBuffer; // receive buffer (for partial messages)
	bool				_autheticated; // client authentification state
	bool				_registered; // client registration state
	// channels
	
public:
	Client(int fd);
	~Client();

	// message parsing & validation methods
	void 				appendToBuffer(const char* data, size_t len);
	bool 				hasCompleteMessage() const;
	std::string 		extractMessage();
	
	// Client authentication & registration methods
	void 				setAuthenticated(bool status);
	void 				setRegister(bool status);
	void 				setNickname(const std::string& str);
	void 				setUsername(const std::string& str);

	bool 				isAuthenticated(void);
	bool 				isRegistered(void);
	const std::string& 	getNickname(void); 
	int 				getFd(void);
};

#endif