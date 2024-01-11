/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   responseHandler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami <aalami@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 18:12:26 by aalami            #+#    #+#             */
/*   Updated: 2024/01/11 16:02:53 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void responseHandler(std::vector<struct pollfd> &fds, int index)
{
    // std::ifstream stream;
    // stream.open("index.html");
    // std::string buffer;
    // getline(stream, buffer, '\0');
    
    std::string response = "HTTP/1.1 302 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: 13\r\n";
    response += "\r\n";
    response += "Hello, World!";
    
    const char *buff = response.c_str();
    send(fds[index].fd, buff, strlen(buff),0 );
}