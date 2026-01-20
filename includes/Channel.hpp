/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gholloco <gwendal.hollocou@orange.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/17 21:11:16 by gholloco          #+#    #+#             */
/*   Updated: 2026/01/20 07:43:11 by gholloco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TODO: KICK MODE INVITE ETC

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

class Client;
# include <set>
# include <string>

class Channel {
private:
	int						_id;
	unsigned int			_userLimit;
	std::string				_topic;
	std::string				_name;
	std::string				_password;
	std::set<char>			_modes; // i, t, k, l
	std::set<Client*>		_members;
	std::set<Client*>		_operators;
	std::set<Client*>		_invited;
	
public:
	Channel(void);
	~Channel(void);

	// getters
	bool					isInviteOnly(void) const;
	bool					isTopicRestricted(void) const;
	bool					hasKey(void) const;
	bool					hasUserLimit(void) const;
	int						getId(void) const;
	unsigned int			getUserLimit(void) const;
	const std::string&		getTopic(void) const;
	const std::string&		getName(void) const;
	const std::string&		getPassword(void) const;

	// setters
	void					setMode(char m, bool b);
	void					setKey(const std::string& key);
	void					unsetKey(void);
	void					setUserLimit(unsigned int limit);
	void					unsetUserLimit(void);
	void					setTopic(const std::string& topic);

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
	void					broadcast(const std::string& msg, Client* c);
	
};

#endif
