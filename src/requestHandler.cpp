/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 17:39:12 by aalami            #+#    #+#             */
/*   Updated: 2024/01/22 19:04:08 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void trimString(std::string &str)
{
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");
    str = str.substr(first, last - first + 1);
}
std::map<std::string, std::string> requestHandler(std::vector<struct pollfd> &fds, int index)
{
    char c;
    // int rec;
    std::string request;
    std::map<std::string, std::string> requestMap;

    while (recv(fds[index].fd, &c, 1, 0) > 0)
    {
        request += c;
    }
    // if (rec < 0)
    //     errorHandler(fds, "Recv");
    // std::cout<<request<<std::endl;
    std::stringstream tmp(request);
    std::string str;
    std::string key;
    std::string value;
    int i = 0;
    char delim;
    while(getline(tmp, str, '\r'))
    {
        if (str.empty())
            continue;
        std::stringstream stream(str);
        if (i == 0)
            delim = ' ';
        else
            delim = ':';
        if (getline(stream, key,delim) && getline(stream, value) && key.size() && value.size())
        {    trimString(key);
            trimString(value);
            requestMap[key] = value;
            // std::cout<<key<<" "<<requestMap[key];

        }
    }
    return requestMap;
}