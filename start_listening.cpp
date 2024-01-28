/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_listening.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaktiwy <hsaktiwy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/14 13:26:32 by adardour          #+#    #+#             */
/*   Updated: 2024/01/28 16:21:35 by hsaktiwy         ###   ########.fr       */
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
    client.CreateResponse(status_codes);
    response const &resp = client.getHttp_response();
    // printf("%s\n", resp.getFile().c_str());
    if (resp.getFile() == "")
    {
        string_response = resp.getHttp_response() + resp.getBody_string();
        int bytes_written = send(poll_fds[i].fd, string_response.c_str(), string_response.size(), 0);
        if (bytes_written < 0)
            perror("write ");
    }
    else
    {
        string_response = resp.getHttp_response();
        int bytes_written = send(poll_fds[i].fd, string_response.c_str(), string_response.size(), 0);
        if (bytes_written < 0)
            perror("write ");
        std::string file = resp.getFile();
        struct stat stat_buff;
        int error = stat(file.c_str(), &stat_buff);
        if (error == 0)
        {
            // printf("+++++++++++++++> %s (%lld)\n", file.c_str(), stat_buff.st_size);
            long long size = stat_buff.st_size;
            std::stringstream ss;
            std::string body_size;
            ss << size;
            ss >> body_size;
            body_size = "Content-Length: " + body_size + "\r\n\r\n";
            send(poll_fds[i].fd, body_size.c_str(), body_size.size(), 0);
            int fd = open(file.c_str(), 0644);
            if (fd > 0)
            {
                char buff[4001];
                std::memset(buff, 0, 4001);
                ssize_t bytes;
                while ((bytes = read(fd, buff, 4001)))
                {
                    send(poll_fds[i].fd, buff, bytes, 0);
                    std::memset(buff, 0, 4001);
                }
                close(fd);
            }
        }
        else
            std::cout << "%%%%%%%%%%%%Error file can't be opend " << file << std::endl;
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

void start_listening_and_accept_request(std::vector<ServerBlocks> &serverBlocks, std::map<unsigned int, std::string> &status_codes)
{
    std::string response;
    std::vector<int> sockets;
    std::vector<struct pollfd> poll_fds;
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
    
    nfds_t size_fd = poll_fds.size();
    while (true)
    {
        int ready = poll(poll_fds.data(), size_fd, -1);
        if (ready < 0)
        {
            perror("poll ");
            exit(0);
        }
        for (size_t i = 0; i < size_fd; i++)
        {
            if ((poll_fds[i].revents & POLLIN))
            {
                if (poll_fds[i].fd == sockets[i])
                {
                    client_socket = create_socket_client(sockets, poll_fds, &size_fd,i);
                    if (client_socket == 35 || client_socket == -1)
                    {
                        if (client_socket == 35)
                            continue;
                        else
                        {
                            std::cout << "Error in accepting connection\n";
                            break;
                        }
                    }
                }
                else
                {
                    Client client;
                    if (poll_fds[i].revents & POLLIN)
                    {
                        if(new_connection(client_socket,new_connections))
                        {
                            printf("new connection from socket %d ...\n",client_socket);
                        }
                        //handle_read(poll_fds, i, &ready_to_write, &size_fd,serverBlocks, response, &flag,&status,human_status, http_request);
                        handle_request(poll_fds,i,&ready_to_write,&size_fd,serverBlocks, response, client, status_codes);
                        CgiEnv cgi(client.getWorker());
                        cgi.setCgiServerName();
                        cgi.setCgiServePort();
                        std::cout<<"SERVER_NAME   : "<<cgi.getCgiServerName()<<" SERVER_PORT :  "<<cgi.getCgiServerPort()<<std::endl;
                        //handle_read(poll_fds, i, &ready_to_write, &size_fd,serverBlocks, response, &flag,&status,human_status,mime_type);
                    }
                    if (poll_fds[i].revents & POLLOUT)
                    {
                        handle_response(poll_fds,i,&ready_to_write, &size_fd,response, &flag,&status,human_status,mime_type, client, status_codes);
                    }
                }
            }
        }
    }
}
