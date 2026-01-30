/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gholloco <gwendal.hollocou@orange.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/17 21:14:56 by gholloco          #+#    #+#             */
/*   Updated: 2026/01/29 20:03:12 by gholloco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

Channel::Channel(std::string &name) : _name(name) {}
Channel::~Channel(){}

// getters

bool Channel::isInviteOnly(void) const
{
	return this->_modes.count('i');
}

bool Channel::isTopicRestricted(void) const
{
	return this->_modes.count('t');
}

bool Channel::hasKey(void) const
{
	return this->_modes.count('k');
}

bool Channel::hasUserLimit(void) const
{
	return this->_modes.count('l');
}

int Channel::getId(void) const
{
	return this->_id;
}

size_t Channel::getMemberCount(void) const
{
	return this->_members.size();
}

unsigned int Channel::getUserLimit(void) const
{
	return this->_userLimit;
}

const std::string& Channel::getTopic(void) const
{
	return this->_topic;
}

const std::string& Channel::getName(void) const
{
	return this->_name;
}

const std::string& Channel::getPassword(void) const
{
	return this->_password;
}

std::string Channel::getMemberList(void) 
{
	std::string list;
	for (std::set<Client*>::iterator it = this->_members.begin(); it != this->_members.end(); ++it)
	{
		if (this->_operators.count(*it))
			list += "@";
		list += (*it)->getNickname();
		list += " ";
	}
	return list;
}

// setters
void Channel::setMode(char m, bool b)
{
	if (b)
		this->_modes.insert(m);
	else
		this->_modes.erase(m);
}

void Channel::setKey(const std::string& key)
{
	this->_modes.insert('k');
	this->_password = key;
}

void Channel::unsetKey(void)
{
	this->_modes.erase('k');
	this->_password.clear();
}

void Channel::setUserLimit(unsigned int limit)
{
	this->_modes.insert('l');
	this->_userLimit = limit;
}

void Channel::unsetUserLimit(void)
{
	this->_modes.erase('l');
	this->_userLimit = 0;
}

void Channel::setTopic(const std::string& topic)
{
	this->_topic = topic;
}

// members

bool Channel::isMember(Client* c) const
{
	return this->_members.count(c) != 0;
}

bool Channel::isOperator(Client* c) const
{
	return this->_operators.count(c) != 0;
}

bool Channel::addMember(Client* c)
{
	return this->_members.insert(c).second;
}

bool Channel::removeMember(Client* c)
{
	this->_operators.erase(c);
	this->_invited.erase(c);
	return this->_members.erase(c) != 0;
}

bool Channel::addOperator(Client* c)
{
	if (!isMember(c))
		return false;
	return this->_operators.insert(c).second;
}

bool Channel::removeOperator(Client* c)
{
	return this->_operators.erase(c) != 0;
}

bool Channel::invite(Client* c)
{
	if (isMember(c))
		return false;
	return this->_invited.insert(c).second;
}

bool Channel::isInvited(Client* c) const
{
	return this->_invited.count(c) != 0;
}

bool Channel::removeInvite(Client* c)
{
	return this->_invited.erase(c) != 0;
}

void Channel::broadcast(Server& srv, const std::string& msg, Client *c)
{
	for (std::set<Client*>::iterator it = this->_members.begin(); it != this->_members.end(); ++it)
	{
		if (*it != c)
			srv.queueToClient(*it, msg + "\r\n"); 
	}
}
