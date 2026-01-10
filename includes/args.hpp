/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mglikenf <mglikenf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 17:39:27 by mglikenf          #+#    #+#             */
/*   Updated: 2026/01/10 17:47:36 by mglikenf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARGS_HPP
# define ARGS_HPP

#include <string>

bool validArgs(int argc, char **argv, int& port, std::string& password);
bool validPort(const char* str, int& port);
bool validPassword(const char* str, std::string& password);
bool isWhitespace(const std::string& str);

#endif