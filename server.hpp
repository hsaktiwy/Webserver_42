/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami <aalami@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/03 10:55:55 by aalami            #+#    #+#             */
/*   Updated: 2024/01/13 17:57:08 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <vector>
#include <map>
#include <fcntl.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

typedef struct s_socket_data
{
    int fd;
    struct sockaddr_in socketAddr;
    
}t_socket_data;

typedef std::vector<int>::iterator intVect_it;
typedef std::map<std::string,std::string>::iterator mapstr_it;
typedef std::vector<t_socket_data>::iterator socket_it;
template <typename T>
void closeAllSockets(T &socketsFds)
{
    for (size_t i = 0; i < socketsFds.size(); i++)
    {
        if (socketsFds[i].fd > 0 )
            close(socketsFds[i].fd);
    }
}
template <typename T>
void errorHandler(T &socketsFds, const char *error)
{
    std::string errortype = error;
    std::string message = errortype + " Error....closing all sockets";
    closeAllSockets(socketsFds);
    throw message.c_str();
}
std::map<std::string, std::string> requestHandler(std::vector<struct pollfd> &fds, int index);
void responseHandler(std::vector<struct pollfd> &fds, int index, char **env,
    std::map<std::string, std::string> &request);

#endif