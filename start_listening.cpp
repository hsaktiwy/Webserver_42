/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_listening.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/14 13:26:32 by adardour          #+#    #+#             */
/*   Updated: 2024/01/24 18:00:42 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"
#include <map>
#include <unistd.h>

bool valid_port(const std::string &port)
{
    int i = 0;
    while (port[i] != '\0')
    {
        if (!isdigit(port[i]))
            return false;
        i++;
    }

    return true;
}
bool parse_each_segment(std::string &segment)
{
    int i = 0;
    while (segment[i] != '\0')
    {
        if (!isdigit(segment[i]))
        {
            return false;
        }
        i++;
    }
    return true;
}

bool parse_port_host(std::string &port,std::string &host)
{
    std::stringstream object_string(host);
    std::string str;
    unsigned int segment = 0;
    while (getline(object_string, str, '.'))
    {
        if (!parse_each_segment(str))
        {
            return (false);
        }
        if (atoi(str.c_str()) > 255 || atoi(str.c_str()) < 0)
        {
            return (false);
        }
        if (str.empty() || str.size() > 3)
        {
            return (false);
        }
        if (str.size() > 1 && str[0] == '0')
        {
            return false;
        }
        segment++;
    }
    int i = 0;
    while (port[i])
    {
        if (!isdigit(port[i]))
        {
            return false;
        }
        i++;
    }
    if (atoi(port.c_str()) < 0 || atoi(port.c_str()) > 65535)
    {
        return false;
    }
    return (segment == 4);
}

void    get_port_host(ServerBlocks &serverBlocks,t_port_host &port_host)
{
    int i = 0;
    std::string str;
    std::string port;
    std::string host;
    std::string ip_address;
    int find = 0;
    while (i < serverBlocks.getDirectives().size())
    {
        if (!serverBlocks.getDirectives()[i].getDirective().compare("listen"))
        {
            str = serverBlocks.getDirectives()[i].getArgument()[0];
            int find = str.find(':');
            port = str.substr(find + 1);
            host = str.substr(0,find);
            if ((atoi(port.c_str()) < 0 || atoi(port.c_str()) > 65535) || !valid_port(port))
            {
                printf("error \n");
                exit(1);
            }
            port_host.port = atoi(port.c_str());
            port_host.host = host;
            break;
        }
        i++;
    }
}

void    create_sockets(std::vector<ServerBlocks> &serverBlocks,std::vector<int> &sockets)
{
    int socket_fd;
    struct addrinfo *result,*p,hints;
    t_port_host port_host;
    int status;
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        memset(&hints,0,sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        get_port_host(serverBlocks[i],port_host);
        if ((status = getaddrinfo(port_host.host.c_str(),std::to_string(port_host.port).c_str(), &hints, &result)) != 0)
        {
            std::cerr << "error: " << gai_strerror(status) << std::endl;
            exit(0);
        }
        for (p = result; p != NULL; p = p->ai_next)
        {
            socket_fd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
            if (socket_fd == -1)
            {
                perror("socket");
                exit(1);
            }
            if(fcntl(socket_fd,F_SETFL, O_NONBLOCK | O_CLOEXEC) < 0)
            {
                perror("set socket ");
                exit(1);
            }
            int opt = 1;
            if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
            {
                perror("set sockopt ");
                exit(1);
                                    
            }
            if(bind(socket_fd,p->ai_addr,p->ai_addrlen) < 0)
            {
                perror("bind socket ");
                exit(1);
                                    
            }
            if (listen(socket_fd, 0) < 0)
            {
                perror("socket ");
                exit(1);
                                    
            }
            sockets.push_back(socket_fd);
            memset(&port_host,0,sizeof(port_host));
        }
    }
}
void    init_poll_fds(std::vector<struct pollfd> &poll_fds,int size,std::vector<int> &sockets)
{
    
    for (int i = 0; i < size; i++)
    {
        struct pollfd temp;
        temp.fd = sockets[i];
        temp.events = POLLIN;
        poll_fds.push_back(temp);
    }
}

int create_socket_client(std::vector<int> &sockets,std::vector<struct pollfd> &poll_fds, nfds_t *size_fd, int i)
{
    int client_socket;
    sockaddr_in client;
    socklen_t socklen = sizeof(client);
    client_socket = accept(sockets[i], (struct sockaddr *)&client, &socklen);
    if (client_socket < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            return 35;
        }
        else
        {
            std::cout << "Error in accepting connection\n";
            return -1;
        }
    }
    else
    {
        struct pollfd temp;
        temp.fd = client_socket;
        temp.events = POLLIN | POLLOUT; 
        poll_fds.push_back(temp);
        (*size_fd)++;
    }
    return (client_socket);
}


void handle_read(std::vector<struct pollfd> &poll_fds, int i, int *ready_to_write, \
                  nfds_t *size_fd, std::vector<ServerBlocks> &serverBlocks, std::string &response, int *flag,int *status,std::string &human_status)
{
    char buffer[1024];
    int bytes_read = recv(poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        parse_request(buffer, serverBlocks, response, flag,status,human_status);
        *ready_to_write = 1;
    }
    // else if (bytes_read == 0)
    // {
    //     printf("close client\n");
    //     close(poll_fds[i].fd);
    //     poll_fds.erase(poll_fds.begin() + i);
    //     (*size_fd)--;
    // }
    else
    {
        perror("recv ");
    exit (0);
    }
}

void handle_response(std::vector<struct pollfd> &poll_fds,int i,int *ready_to_write, nfds_t *size_fd,std::string &response,int *flag,int *status,std::string &human_status)
{
    int length = response.length();
    
    std::string statusLine = "HTTP/1.1 200 OK\r\n";
    std::string contentTypeHeader = "Content-Type: text/plain\r\n";
    std::string contentLengthHeader = "Content-Length: 13\r\n";
    std::string responseBody = "Hello, World!\r\n";

    // Construct the complete HTTP response
    std::ostringstream responseStream;
    responseStream << statusLine << contentTypeHeader << contentLengthHeader << "\r\n" << responseBody;
    
    response = responseStream.str();
    int bytes_written = write(poll_fds[i].fd, response.c_str(), response.size());
    response.clear();
    if (bytes_written < 0)
    {
        perror("write ");
    }
    (*ready_to_write) = 0;
    close(poll_fds[i].fd);
    poll_fds.erase(poll_fds.begin() + i);
    (*size_fd)--;
}

int    new_connection(int client_socket,std::vector<int> new_connections)
{
    for (size_t i = 0; i < new_connections.size(); i++)
    {
        if (new_connections[i] == client_socket)
        {
            return (0);
        }
    }
    return (1);
}

void start_listening_and_accept_request(std::vector<ServerBlocks> &serverBlocks)
{
    std::string response;
    std::vector<int> sockets;
    std::vector<t_socket_data> socketsFds;
    std::vector<int> vecFds; // save only opened sockets on a vector (useful when working with poll)
    std::vector<struct pollfd> poll_fds;
    struct pollfd tmp; 
    // int timeout = 2 * 60 * 1000;
    int pollRet = 0;
    int acceptRet = 0;
    char buffer[1024];
    int client_socket = -1;
    int ready_to_write = 0;
    int flag = 0;
    int status;
    std::string human_status;

    create_sockets(serverBlocks, sockets);
    init_poll_fds(poll_fds, serverBlocks.size(), sockets);
    std::vector<int> new_connections;
    
    nfds_t size_fd = poll_fds.size();
    
    std::cout<<GREEN<<"Waiting for an incomming request"<<RESET<<std::endl;
    while (true)
    {
        pollRet = poll(poll_fds.data(), poll_fds.size(), TIME_OUT);
        if (pollRet < 0)
        {
            perror("poll ");
            exit(0);
        }
        if (pollRet == 0)
        {
            std::cout<<RED<<"Connection timeout...."<<RESET<<std::endl;
            continue;
        }
            
        for (size_t i = 0; i < poll_fds.size(); i++)
        {
            if (poll_fds[i].revents == 0)
                continue;
            else if (poll_fds[i].fd == sockets[i])
            {
                socklen_t len;
                struct sockaddr_in tmpAddr;
                len = sizeof(tmpAddr);
                memset(&tmpAddr, 0, len);
                
                acceptRet = accept(poll_fds[i].fd, (struct sockaddr *)&tmpAddr, &len);
                // if (acceptRet < 0)
                //     break;
                char clientIP[INET_ADDRSTRLEN];
                unsigned short clientPort = ntohs(tmpAddr.sin_port);
                inet_ntop(AF_INET, &(tmpAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
                std::cout<<GREEN <<"New incomming connection from the client Socket ";
                std::cout<<acceptRet<<" with the address : "<<clientIP<<":"<<clientPort<< " To the socket "<<poll_fds[i].fd<<RESET<<std::endl;
                    // if (fcntl(acceptRet, F_SETFL, O_NONBLOCK | FD_CLOEXEC) < 0)
                    //     errorHandler(socketsFds, "Fcntl");
                    memset(&tmp, 0, sizeof(tmp));
                    tmp.fd = acceptRet; // add a client socket to the poll array
                    tmp.events = POLLIN; // waiting for I/O on this socket
                    poll_fds.push_back(tmp); 
                    // pollFdsSize++;
            }
            else
            {
                // if (poll_fds[i].revents == 0)
                //     continue;
                if (poll_fds[i].revents & POLLIN)
                {
                    // printf("DDDd\n");
                    // request = requestHandler(fds, i);
                    // char c;
                    // read(fds[i].fd,&c, 1);
                    handle_read(poll_fds, i, &ready_to_write, &size_fd,serverBlocks, response, &flag,&status,human_status);
                    std::cout<<YELLOW<<"Request sent from Client "<<poll_fds[i].fd<<RESET<<std::endl;
                    poll_fds[i].events = POLLOUT;
                        // fds[i].events = POLLOUT;
                }
                else if (poll_fds[i].revents & POLLOUT)
                {
                    handle_response(poll_fds,i,&ready_to_write, &size_fd,response, &flag,&status,human_status);
                    std::cout<<BLUE<<"Response sent to: " <<poll_fds[i].fd<<" !!"<<RESET<<std::endl;
                    std::cout<<YELLOW<<"Connection to Client "<<poll_fds[i].fd<<" closed"<<RESET<<std::endl;
                    // close(poll_fds[i].fd);
                    // poll_fds.erase(poll_fds.begin() + i);
                    // pollFdsSize--;
                    // fds[i].events = POLLIN;
                }
                else
                {
                    std::cout<<RED<<"Client "<<poll_fds[i].fd<<" closed the connection"<<RESET<<std::endl;
                    close (poll_fds[i].fd);
                    poll_fds.erase(poll_fds.begin() + i);
                }
            }
            // if ((poll_fds[i].revents & POLLIN))
            // {
            //     if (poll_fds[i].fd == sockets[i])
            //     {
            //         client_socket = create_socket_client(sockets, poll_fds, &size_fd,i);
            //         if (client_socket == 35 || client_socket == -1)
            //         {
            //             if (client_socket == 35)
            //                 continue;
            //             else
            //             {
            //                 std::cout << "Error in accepting connection\n";
            //                 break;
            //             }
            //         }
            //     }
            //     else
            //     {
            //         if (poll_fds[i].revents & POLLIN)
            //         {
            //             if(new_connection(client_socket,new_connections))
            //             {
            //                 printf("new connection from socket %d ...\n",client_socket);
            //             }
            //             handle_read(poll_fds, i, &ready_to_write, &size_fd,serverBlocks, response, &flag,&status,human_status);
            //         }
            //         if (poll_fds[i].revents & POLLOUT)
            //         {
            //             handle_response(poll_fds,i,&ready_to_write, &size_fd,response, &flag,&status,human_status);
                        
            //         }
            //     }
            // }
        }
    }
}
