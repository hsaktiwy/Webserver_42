/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami <aalami@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/03 10:54:52 by aalami            #+#    #+#             */
/*   Updated: 2024/01/13 17:57:27 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void closeAllSockets(std::vector<t_socket_data> &socketsFds)
{
    for (size_t i = 0; i < socketsFds.size(); i++)
    {
        if (socketsFds[i].fd > 0 )
            close(socketsFds[i].fd);
    }
}
void errorHandler(std::vector<t_socket_data> &socketsFds, const char *error)
{
    std::string errortype = error;
    std::string message = errortype + " Error....closing all sockets";
    closeAllSockets(socketsFds);
    throw message.c_str();
}
bool createSockets(int sn, std::vector<t_socket_data> &socketsFds)
{
    try
    {
        int i = 0;
        size_t portInit = 4000;
        t_socket_data tmp;
        while (i < sn)
        {
            int opt = 1;
            tmp.fd = socket(AF_INET, SOCK_STREAM,0);
            if (tmp.fd < 0)
                errorHandler(socketsFds, "Socket");
            setsockopt(tmp.fd ,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt));
            if (fcntl(tmp.fd, F_SETFL, O_NONBLOCK | FD_CLOEXEC) < 0)
                errorHandler(socketsFds, "Fcntl");
            tmp.socketAddr.sin_family = AF_INET;
            tmp.socketAddr.sin_port = htons(portInit + i);
            tmp.socketAddr.sin_addr.s_addr = INADDR_ANY;
            if (bind(tmp.fd, (struct sockaddr *)&tmp.socketAddr, sizeof(tmp.socketAddr)) < 0)
                errorHandler(socketsFds, "Binding");
            if (listen(tmp.fd, 128)< 0)
                errorHandler(socketsFds, "Listen ");
            socketsFds.push_back(tmp);
            std::cout<<GREEN<<"new Socket Created: [Socket Descriptor : "<<tmp.fd;
            std::cout<<"] listening on port ["<< portInit + i<<"]"<<RESET<<std::endl;
            memset(&tmp, 0, sizeof(tmp));
            i++;
        }
    }
    catch(const char *e)
    {
        std::cerr <<RED << e << RESET <<'\n';
        return false;
    }
    return true;
}

socket_it searchForSocketByfd(std::vector<t_socket_data> &socketsFds, int fd)
{
    socket_it it;
    for(it = socketsFds.begin(); it < socketsFds.end(); it++)
    {
        if (it->fd == fd)
            break;
    }
    return it;
}
void waitForIncommingRequests(std::vector<t_socket_data> &socketsFds, char **env)
{
    std::vector<struct pollfd> fds;
    std::vector<int> vecFds; // save only opened sockets on a vector (useful when working with poll)
    struct pollfd tmp; 
    size_t socketQueueSize = socketsFds.size(); // hold the size of all the sockets waiting for requests
    int timeout = 0.3 * 60 * 1000;
    int pollRet = 0;
    int acceptRet = 0;
    for (size_t i = 0; i < socketQueueSize; i++)
    {
        tmp.fd = socketsFds[i].fd;
        tmp.events = POLLIN; // socket wait for a readable data
        fds.push_back(tmp);
        vecFds.push_back(socketsFds[i].fd); // save fds 
        memset(&tmp, 0, sizeof(tmp));
    }
    std::map<std::string, std::string> request;
    size_t pollFdsSize = socketQueueSize; //intialize the size of pollfds by the number of sockets opened
    std::cout<<GREEN<<"Waiting for an incomming request"<<RESET<<std::endl;
    while (1)
    {
        pollRet = poll(fds.data(), pollFdsSize, timeout);
        if (pollRet < 0)
            errorHandler(socketsFds, "Poll");
        if (pollRet == 0)
        {
            std::cout<<GREEN<<"Connection timeout...."<<RESET<<std::endl;
            continue;
        }
        for(size_t i = 0; i < pollFdsSize; i++)
        {
            if (fds[i].revents == 0)
                continue;
            else if (std::find(vecFds.begin(),vecFds.end(), fds[i].fd) != vecFds.end())
            {
                socket_it it = searchForSocketByfd(socketsFds, fds[i].fd);
                socklen_t len = sizeof(it->socketAddr);
                 struct sockaddr_in tmpAddr;
                if (it == socketsFds.end())
                    exit (0);
                    acceptRet = accept(fds[i].fd, (struct sockaddr *)&tmpAddr, &len);
                    if (acceptRet < 0)
                        break;
                    std::cout<<GREEN<<"New incomming connection from the client Socket ";
                    std::cout<<acceptRet<< " To the socket "<<fds[i].fd<<RESET<<std::endl;
                    // if (fcntl(acceptRet, F_SETFL, O_NONBLOCK | FD_CLOEXEC) < 0)
                    //     errorHandler(socketsFds, "Fcntl");
                    tmp.fd = acceptRet; // add a client socket to the poll array
                    tmp.events = POLLIN; // waiting for readable data on this socket
                    fds.push_back(tmp); 
                    pollFdsSize++;
            }
            else
            {
                if (fds[i].revents & POLLIN)
                {
                    std::cout<<YELLOW<<"Request sent from Client "<<fds[i].fd<<RESET<<std::endl;
                    request = requestHandler(fds, i);
                        fds[i].events = POLLOUT;
                }
                else if (fds[i].revents & POLLOUT)
                {
                    // std::cout<<YELLOW<<"Client "<<fds[i].fd<<" Ready to accept a response....."<<RESET<<std::endl;
                    // if (f == 0)
                        responseHandler(fds, i, env, request);
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    pollFdsSize--;
                    // fds[i].events = POLLIN;
                    std::cout<<BLUE<<"Response sent to: " <<fds[i].fd<<" !!"<<RESET<<std::endl;
                }
                else
                {
                    std::cout<<YELLOW<<"Client "<<fds[i].fd<<" closed the connection"<<RESET<<std::endl;
                    close (fds[i].fd);
                    fds.erase(fds.begin() + i);
                    pollFdsSize--;
                }
            }
        }
    }
}
int main(int argc, char **argv, char **env)
{
    argc = 0;
    (void )argv;
    std::vector<t_socket_data> socketsFds;
    if(createSockets(2, socketsFds))
    {
        waitForIncommingRequests(socketsFds, env);
        
    }
}
