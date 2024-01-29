/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_listening.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaktiwy <hsaktiwy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/14 13:26:32 by adardour          #+#    #+#             */
/*   Updated: 2024/01/29 17:06:16 by hsaktiwy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"
#include "Client.hpp"
#include <map>
#include <unistd.h>
#include "cgi/cgi.hpp"

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

// void handle_read(std::vector<struct pollfd> &poll_fds, int i, int *ready_to_write, \
//                   nfds_t *size_fd, std::vector<ServerBlocks> &serverBlocks, std::string &response, int *flag,int *status,std::string &human_status,std::string &mime_type)
// {
//     char buffer[1024];
//     int bytes_read = recv(poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
//     if (bytes_read > 0)
//     {
//         buffer[bytes_read] = '\0';
//         init_worker_block(buffer, serverBlocks);
//         *ready_to_write = 1;
//     }
//     else if (bytes_read == 0)
//     {
//         printf("close client\n");
//         close(poll_fds[i].fd);
//         poll_fds.erase(poll_fds.begin() + i);
//         (*size_fd)--;
//     }
//     else
//     {
//         perror("recv ");
//     }
// }

void    handle_request(std::vector<struct pollfd> &poll_fds,int i,int *ready_to_write, nfds_t *size_fd,std::vector<ServerBlocks> &serverBlocks,std::string &response, Client & client, std::map<unsigned int, std::string> &status_codes)
{
    char buffer[1024];
    int bytes_read = recv(poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        client.ParseRequest(buffer, serverBlocks);
    }
    else if (bytes_read == 0)
    {
        printf("close client\n");
        close(poll_fds[i].fd);
        poll_fds.erase(poll_fds.begin() + i);
        (*size_fd)--;
    }
    else
    {
        perror("recv ");
    }
}

void handle_response(std::vector<struct pollfd> &poll_fds,int i,int *ready_to_write, nfds_t *size_fd,std::string &string_response,int *flag,int *status,std::string &human_status,std::string &mime_type, Client & client, std::map<unsigned int, std::string> &status_codes)
{
    // printf("client %s\n", client.getHttp_request().getReq().c_str());
    // printf("---%lld %lu\n", client.getHttp_response().getHeader_size(), client.getHttp_response().getHeader_index());
    response &resp = (response &)client.getHttp_response();
    printf("%lld %lu BodySize %lld BodyIndex %lu\n", resp.getHeader_size(), resp.getHeader_index(), resp.getBody_size(), resp.getBody_index());
    client.CreateResponse(status_codes);
    if (resp.getFile() == "")
    {
        // string_response = resp.getHttp_response() + resp.getBody_string();
        // std::cout<<string_response<<std::endl;
        // printf("In Response File step\n");
        size_t writeBytes = CHUNK_SIZE;
        // printf("@@@@Header sent %d\n", resp.getHeader_sent());
        if (resp.getHeader_sent() == false)
        {
            long long Hsize = resp.getHeader_size();
            long long Hindex = resp.getHeader_index();

            if (Hindex < Hsize)
            {
                printf("%lld %lld\n", Hsize, Hindex);
                size_t bytes = writeBytes;
                long long rest = Hsize - Hindex;
                if (rest <  CHUNK_SIZE)
                    bytes = rest;
                printf("rest %lld\n", rest);
                ssize_t bytes_written = send(poll_fds[i].fd, &resp.getHttp_response()[Hindex], bytes, 0);
                resp.setHeader_index(Hindex + bytes);
                if (resp.getHeader_size() == resp.getHeader_index())
                    resp.setHeader_sent(true);
                writeBytes -= bytes;
                if (bytes_written < 0)
                    perror("write ");
            }
        }
        if (resp.getBody_sent() == false && writeBytes > 0)
        {
            long long Bsize = resp.getBody_size();
            long long Bindex = resp.getBody_index();

            if (Bindex < Bsize)
            {
                size_t bytes = writeBytes;
                long long rest = Bsize - Bindex;
                if (rest <  CHUNK_SIZE)
                    bytes = rest;
                ssize_t bytes_written = send(poll_fds[i].fd, &resp.getBody_string()[Bindex], bytes, 0);
                resp.setBody_index(Bindex + bytes);
                if (resp.getBody_size() == resp.getBody_index())
                    resp.setBody_sent(true);
                writeBytes -= bytes; 
                if (bytes_written < 0)
                    perror("write ");
            }
        }
        if (resp.getBody_sent() == false && resp.getBody_size() == -1)
            resp.setBody_sent(true);
    }
    else
    {
        // printf("In Response File step\n");
        size_t writeBytes = CHUNK_SIZE;
        // printf("Header sent %d\n", resp.getHeader_sent());
        if (resp.getHeader_sent() == false)
        {
            long long Hsize = resp.getHeader_size();
            long long Hindex = resp.getHeader_index();

            // printf("%lld %lld", Hsize, Hindex);
            if (Hindex < Hsize)
            {
                // printf("In File %s\n", &resp.getHttp_response()[Hindex]);
                size_t bytes = writeBytes;
                long long rest = Hsize - Hindex;
                if (rest <  CHUNK_SIZE)
                    bytes = rest;
                // printf("rest in file %lld\n", rest);
                ssize_t bytes_written = send(poll_fds[i].fd, &resp.getHttp_response()[Hindex], bytes, 0);
                if (bytes_written < 0)
                    perror("write ");
                resp.setHeader_index(Hindex + bytes);
                if (resp.getHeader_size() == resp.getHeader_index())
                    resp.setHeader_sent(true);
                writeBytes -= bytes;
            }
        }
        // printf("Body sent %d\n", resp.getBody_sent());
        if (resp.getBody_sent() == false && writeBytes > 0)
        {
            std::string file = resp.getFile();
            if (resp.getFileOpened() == false)
            {
                struct stat stat_buff;
                int error = stat(file.c_str(), &stat_buff);
                if (error == 0)
                {
                    long long size = stat_buff.st_size;
                    std::stringstream ss;
                    std::string body_size;
                    ss << size;
                    ss >> body_size;
                    body_size = "Content-Length: " + body_size + "\r\nAccept-Ranges: none\r\n\r\n";

                    resp.setBody_size(size + body_size.size());
                    send(poll_fds[i].fd, body_size.c_str(), body_size.size(), 0);
                    size_t Bindex = resp.getBody_index();
                    resp.setBody_index(Bindex + body_size.size());
                    // printf("Body size %lld, body_index %lu\n", resp.getBody_size(), resp.getBody_index());
                    if (resp.getBody_size() == resp.getBody_index())
                        resp.setBody_sent(true);
                    writeBytes -= body_size.size();
                }
            }
            // printf("Body sent2 %d\n", resp.getBody_sent());
            if (resp.getBody_sent() == false && resp.getBody_size() != -1 && writeBytes > 0)
            {
                int fd = resp.getFd();
                // printf("fd == %d\n", fd);
                if (fd == -1)
                    fd = open(file.c_str(), 0644), resp.setFd(fd), resp.setFileOpened(true);
                if (fd > 0)
                {
                    char buff[writeBytes];
                    std::memset(buff, 0, writeBytes);
                    ssize_t bytes = read(fd, buff, writeBytes);
                    if (bytes > 0)
                    {
                        send(poll_fds[i].fd, buff, bytes, 0);
                        size_t Bindex = resp.getBody_index();
                        resp.setBody_index(Bindex + bytes);
                        if (resp.getBody_size() == resp.getBody_index())
                            resp.setBody_sent(true), close(fd);
                    }
                    else
                        write(2, "Something Went Wrong!\n", 22);
                }
            }
            // static int st;
            // if (st == 30)
            //     exit(0);
            // st++;
        }
    }
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

// void start_listening_and_accept_request(std::vector<ServerBlocks> &serverBlocks, std::map<unsigned int, std::string> &status_codes)
// {
//     std::string response;
//     std::vector<int> sockets;
//     std::vector<struct pollfd> poll_fds;
//     char buffer[1024];
//     int client_socket = -1;
//     int ready_to_write = 0;
//     int flag = 0;
//     int status;
//     std::string human_status;
//     std::string mime_type;

//     create_sockets(serverBlocks, sockets);
//     init_poll_fds(poll_fds, serverBlocks.size(), sockets);
//     std::vector<int> new_connections;
//     std::vector<Client> clients;
//     nfds_t size_fd = poll_fds.size();
//     while (true)
//     {
//         int ready = poll(poll_fds.data(), size_fd, -1);
//         if (ready < 0)
//         {
//             perror("poll ");
//             exit(0);
//         }
//         for (size_t i = 0; i < size_fd; i++)
//         {
//             if ((poll_fds[i].revents & POLLIN))
//             {
//                 if (poll_fds[i].fd == sockets[i])
//                 {
//                     client_socket = create_socket_client(sockets, poll_fds, &size_fd,i);
                    
//                     if (client_socket == 35 || client_socket == -1)
//                     {
//                         if (client_socket == 35)
//                             continue;
//                         else
//                         {
//                             std::cout << "Error in accepting connection\n";
//                             break;
//                         }
//                     }
//                 }
//                 else
//                 {
//                     Client client;
//                     if (poll_fds[i].revents & POLLIN)
//                     {
//                         if(new_connection(client_socket,new_connections))
//                         {
//                             printf("new connection from socket %d ...\n",client_socket);
//                         }
//                         //handle_read(poll_fds, i, &ready_to_write, &size_fd,serverBlocks, response, &flag,&status,human_status, http_request);
//                         handle_request(poll_fds,i,&ready_to_write,&size_fd,serverBlocks, response, client, status_codes);
//                         // CgiEnv cgi(client.getWorker());
//                         // cgi.setCgiServerName();
//                         // cgi.setCgiServePort();
//                         // cgi.setCgiScriptPath();
//                         // std::cout<<"SERVER_NAME   : "<<cgi.getCgiServerName()<<" SERVER_PORT :  "<<cgi.getCgiServerPort()<<std::endl;
//                         //handle_read(poll_fds, i, &ready_to_write, &size_fd,serverBlocks, response, &flag,&status,human_status,mime_type);
//                     }
//                     if (poll_fds[i].revents & POLLOUT)
//                     {
//                         handle_response(poll_fds,i,&ready_to_write, &size_fd,response, &flag,&status,human_status,mime_type, client, status_codes);
//                     }
//                 }
//             }
//         }
//     }
// }

size_t findClientBySocketFd(std::vector<Client> &ClientsVector, int fd)
{
    size_t i = 0;
    
    for (i = 0 ; i <  ClientsVector.size(); i++)
    {
        // printf("Socket id  : %d\n", it->getClientSocket());
        if (ClientsVector[i].getClientSocket() == fd)
            break;
    }
    return i;
}
void start_listening_and_accept_request(std::vector<ServerBlocks> &serverBlocks,std::map<unsigned int, std::string> &status_codes)
{
    std::string response;
    std::vector<int> sockets;
    // std::vector<t_socket_data> socketsFds;
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
    std::string mime_type;

    create_sockets(serverBlocks, sockets);
    init_poll_fds(poll_fds, serverBlocks.size(), sockets);
    std::vector<int> new_connections;
    std::vector<Client> ClientsVector;
    
    nfds_t size_fd = poll_fds.size();
    
    std::cout<<GREEN<<"Waiting for an incomming request"<<RESET<<std::endl;
    while (true)
    {
        printf("poool\n");
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
            if (poll_fds[i].fd == sockets[i] && poll_fds[i].revents & POLLIN)
            {
                Client client;
                socklen_t len;
                struct sockaddr_in tmpAddr;
                len = sizeof(tmpAddr);
                memset(&tmpAddr, 0, len);
                
                acceptRet = accept(sockets[i], (struct sockaddr *)&tmpAddr, &len);
                // if (acceptRet < 0)
                //     break;
                client.setClientSocket(acceptRet);
                printf("lol %lld %lu\n", client.getHttp_response().getHeader_size(),client.getHttp_response().getHeader_index());
                ClientsVector.push_back(client);
                char clientIP[INET_ADDRSTRLEN];
                unsigned short clientPort = ntohs(tmpAddr.sin_port);
                inet_ntop(AF_INET, &(tmpAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
                std::cout<<GREEN <<"New incomming connection from the client Socket ";
                std::cout<<acceptRet<<" with the address : "<<clientIP<<":"<<clientPort<< " To the socket "<<poll_fds[i].fd<<RESET<<std::endl;
                    if (fcntl(acceptRet, F_SETFL, O_NONBLOCK | FD_CLOEXEC) < 0)
                    {    perror("fctnl");
                        exit(1);}
                    memset(&tmp, 0, sizeof(tmp));
                    tmp.fd = acceptRet; // add a client socket to the poll array
                    tmp.events = POLLIN; // waiting for I/O on this socket
                    poll_fds.push_back(tmp); 
                    // pollFdsSize++;
            }
            else if (poll_fds[i].fd != sockets[i])
            {
                // if (poll_fds[i].revents == 0)
                //     continue;
                size_t client_it;
                client_it = findClientBySocketFd(ClientsVector, poll_fds[i].fd);
                if (client_it == ClientsVector.size())
                {
                    printf("OUT\n");
                    exit(0);
                    continue ;
                }
                if (poll_fds[i].revents & POLLIN)
                {
                    // printf("DDDd\n");
                    // request = requestHandler(fds, i);
                    // char c;
                    // read(fds[i].fd,&c, 1);
                    handle_request(poll_fds,i,&ready_to_write,&size_fd,serverBlocks, response, ClientsVector[client_it], status_codes);
                    std::cout<<YELLOW<<"Request sent from Client "<<poll_fds[i].fd<<RESET<<std::endl;
                    poll_fds[i].events = POLLOUT;
                    // printf("1-%lld %lu\n", ClientsVector[client_it].getHttp_response().getHeader_size(), ClientsVector[client_it].getHttp_response().getHeader_index());

                        // fds[i].events = POLLOUT;
                }
                else if (poll_fds[i].revents & POLLOUT)
                {
                    // printf("2-%lld %lu\n", ClientsVector[client_it].getHttp_response().getHeader_size(), ClientsVector[client_it].getHttp_response().getHeader_index());

                    handle_response(poll_fds,i,&ready_to_write, &size_fd,response, &flag,&status,human_status,mime_type, ClientsVector[client_it], status_codes);
                    // exit(0);

                    if (ClientsVector[client_it].getHttp_response().getBody_sent() && ClientsVector[client_it].getHttp_response().getHeader_sent())
                    {
                        std::cout<<BLUE<<"Response sent to: " <<poll_fds[i].fd<<" !!"<<RESET<<std::endl;
                        std::cout<<YELLOW<<"Connection to Client "<<poll_fds[i].fd<<" closed"<<RESET<<std::endl;
                        ClientsVector.erase(ClientsVector.begin() + client_it);
                        close(poll_fds[i].fd);
                        poll_fds.erase(poll_fds.begin() + i);
                        i--;
                    }
                    // pollFdsSize--;
                    // fds[i].events = POLLIN;
                }
                // else
                // {
                //     std::cout<<RED<<"Client "<<poll_fds[i].fd<<" closed the connection"<<RESET<<std::endl;
                //     close (poll_fds[i].fd);
                //     poll_fds.erase(poll_fds.begin() + i);
                // }
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