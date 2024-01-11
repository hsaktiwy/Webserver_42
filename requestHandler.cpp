/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami <aalami@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 17:39:12 by aalami            #+#    #+#             */
/*   Updated: 2024/01/04 19:17:52 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void requestHandler(std::vector<struct pollfd> &fds, int index)
{
    char c;
    // int rec;
    std::string request;

    while (recv(fds[index].fd, &c, 1, 0) > 0)
    {
        request += c;
    }
    // if (rec < 0)
    //     errorHandler(fds, "Recv");
    std::cout<<request<<std::endl;
    // printf("%s\n", buffer);
}