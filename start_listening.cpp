/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_listening.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/14 13:26:32 by adardour          #+#    #+#             */
/*   Updated: 2023/12/30 18:17:46 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

void    get_port_host(ServerBlocks &serverBlocks,t_port_host &port_host)
{
    int i = 0;
    std::string str;
    int find = 0;
    while (i < serverBlocks.getDirectives().size())
    {
        if (!serverBlocks.getDirectives()[i].getDirective().compare("listen"))
        {
            str = serverBlocks.getDirectives()[i].getArgument()[0];
            int find = str.find(':');
            port_host.port = atoi(str.substr(find + 1).c_str());
            memset(&port_host.sin_addr, 0, sizeof(port_host.sin_addr));
            inet_pton(AF_INET, str.substr(0,find).c_str(), &port_host.sin_addr);
        }
        i++;
    }
}


void    handle_request(unsigned int clientSocket)
{
    char buffer[1024];
    int bytesRead = read(clientSocket, buffer, 1024);
    buffer[bytesRead] = '\0';
    printf("received: %s\n", buffer);
}

void    start_listening_and_accept_request(std::vector<ServerBlocks> &serverBlocks)
{
    int serverSocket;
    std::vector<int> sockets;
    int clientSocket;
    std::vector<struct pollfd> pollfds(serverBlocks.size());
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        t_port_host port_host;
        get_port_host(serverBlocks[i],port_host);
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (fcntl(serverSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
        {
            perror("");
            exit(1);   
        }
        if (serverSocket == -1) {
            std::cerr << "Error in creating socket\n";
            exit(EXIT_FAILURE);
        }
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port_host.port);
        serverAddr.sin_addr.s_addr = port_host.sin_addr.s_addr;

        int option = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
            std::cerr << "Error setting socket options\n";
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
        if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
            std::cerr << "Error in binding\n";
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
        
        if (listen(serverSocket, SOMAXCONN) == -1)
        {
            std::cerr << "Error in listening\n";
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
        sockets.push_back(serverSocket);
        pollfds[i].fd = sockets[i];
        pollfds[i].events = POLLIN;
        pollfds[i].revents = 0;  
    }
    
    while (true)
    {
        int ret = poll(&pollfds[0], serverBlocks.size(), -1);
        if (ret == -1)
        {
            perror("poll");
            exit(1);
        }
        for (size_t i = 0; i < serverBlocks.size(); i++)
        {
            if (pollfds[i].revents & POLLIN) {
                if (pollfds[i].fd != -1) {
                        struct sockaddr_in clientAddr;
                        socklen_t clientAddrLen = sizeof(clientAddr);
                        clientSocket = accept(pollfds[i].fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
                        if (clientSocket == -1)
                        {
                            if (errno == EWOULDBLOCK || errno == EAGAIN)
                            {
                                continue;
                            } else
                            {
                                std::cerr << "Error in accepting connection\n";
                                break;
                            }
                        }
                        else
                        {   
                            handle_request(clientSocket);
                            close(clientSocket);
                        }
                    }
                }
            }
    }

    for (size_t i = 0; i < sockets.size(); i++)
    {
        close(sockets[i]);
    }
}