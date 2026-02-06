/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/17 21:11:16 by gholloco          #+#    #+#             */
/*   Updated: 2026/02/06 13:50:27 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

class Server;
class Client;
#include <set>
#include <string>
#include <ctime>

class Channel {
private:
	// int						_id;
	std::string				_name;
	std::string				_password;
	std::string				_topic;
	std::string				_topicSetter;
	time_t					_topicTime;
	unsigned int			_userLimit; // unused?
	std::set<char>			_modes; // i, t, k, l
	std::set<Client*>		_members;
	std::set<Client*>		_operators;
	std::set<Client*>		_invited;
	
public:
	Channel(std::string &name);
	~Channel(void);

	// getters
	bool					isInviteOnly(void) const;
	bool					isTopicRestricted(void) const;
	bool					hasKey(void) const;
	bool					hasUserLimit(void) const;
	// int						getId(void) const; - unused
	size_t					getMemberCount(void) const;
	unsigned int			getUserLimit(void) const;
	std::string 			getMemberList(void);
	const std::string&		getTopic(void) const;
	const std::string&		getName(void) const;
	const std::string&		getPassword(void) const;
	const std::string&		getTopicSetter(void) const;
	time_t					getTopicTime(void) const;

	// setters
	void					setMode(char m, bool b);
	void					setKey(const std::string& key);
	void					unsetKey(void);
	void					setUserLimit(unsigned int limit);
	void					unsetUserLimit(void);
	void					setTopic(const std::string& topic, const std::string& setter);

	// members
	bool					isMember(Client* c) const;
	bool					isOperator(Client* c) const;
	bool					addMember(Client* c);
	bool					removeMember(Client* c);
	bool					addOperator(Client* c);
	bool					removeOperator(Client* c);
	bool					invite(Client* c);
	bool					isInvited(Client* c) const;
	bool					removeInvite(Client* c); 
	void					broadcast(Server& srv, const std::string& msg, Client* c);
};

#endif