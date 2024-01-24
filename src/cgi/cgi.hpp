/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 12:14:06 by aalami            #+#    #+#             */
/*   Updated: 2024/01/24 15:15:12 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP
#include "../server.hpp"
class CgiEnv
{
private:
    std::map<std::string, std::string> envMap;
public:
    CgiEnv();
    ~cgi();
};

cgi::cgi(/* args */)
{
}

cgi::~cgi()
{
}

class CgiHnadler
{
    private:
        std::map<s
}
#endif