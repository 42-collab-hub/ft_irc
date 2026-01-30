/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 08:38:00 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/29 06:26:44 by gholloco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <sys/socket.h>
#include <vector>
#include <string>

class Client {
private:
	int					_fd;
	std::string			_nickname;
	std::string			_username;
	std::string			_hostname;
	std::string			_realname;
	std::string			_recvBuffer; // receive buffer (for partial messages)
	std::string			_sendBuffer; // send buffer
	bool				_autheticated; // client authentification state
	bool				_registered; // client registration state
	// channels
	
public:
	Client(int fd, const std::string& hostname);
	~Client();

	// message parsing & validation methods
	void 				appendToBuffer(const char* data, size_t len);
	bool 				hasCompleteMessage() const;
	std::string 		extractMessage();

	// message sending
	void 				queueMessage(const std::string& msg);
	void 				flushMessage(void);
	bool				hasQueuedMessage(void) const;
	
	// Client authentication & registration methods
	void 				setAuthenticated(bool status);
	void 				setRegister(bool status);
	void 				setNickname(const std::string& str);
	void 				setUsername(const std::string& str);
	void				setRealname(const std::string& str);
	void				setHostname(const std::string& str);
	
	bool 				isAuthenticated(void);
	bool 				isRegistered(void);
	std::string			getPrefix(void);
	const std::string& 	getNickname(void);
	const std::string& 	getUsername(void); 
	const std::string& 	getRealname(void);
	const std::string& 	getHostname(void);
	int 				getFd(void);
};

#endif
