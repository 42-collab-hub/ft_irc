/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 08:38:00 by mglikenf          #+#    #+#             */
/*   Updated: 2026/02/07 14:26:57 by mglikenf         ###   ########.fr       */
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
	std::string			_recvBuffer;
	std::string			_sendBuffer;
	bool				_autheticated;
	bool				_registered;
	
public:
	Client(int fd, const std::string& hostname);
	~Client();

	void 				appendToBuffer(const char* data, size_t len);
	bool 				hasCompleteMessage() const;
	std::string 		extractMessage();
	void 				queueMessage(const std::string& msg);
	void 				flushMessage(void);
	bool				hasQueuedMessage(void) const;
	void 				setAuthenticated(bool status);
	void 				setRegister(bool status);
	void 				setNickname(const std::string& str);
	void 				setUsername(const std::string& str);
	void				setRealname(const std::string& str);
	void				setHostname(const std::string& str);
	bool 				isAuthenticated(void) const;
	bool 				isRegistered(void) const;
	std::string			getPrefix(void) const;
	const std::string& 	getNickname(void) const;
	const std::string& 	getUsername(void) const; 
	const std::string& 	getRealname(void) const;
	const std::string& 	getHostname(void) const;
	int 				getFd(void) const;
};

#endif