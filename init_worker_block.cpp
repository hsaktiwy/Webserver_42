/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_worker_block.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 19:33:06 by adardour          #+#    #+#             */
/*   Updated: 2024/01/19 20:26:56 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"



void    setAllowedmethods(Worker &worker, std::vector<Directives> &directives)
{
    for (size_t i = 0; i < directives.size(); i++)
    {
        if (!directives[i].getDirective().compare("allow_methods"))
        {
            worker.setMethod(directives[i].getArgument());
            return;
        }
    }
}

void    setErrorPages(Worker &worker, std::vector<Directives> &directives)
{
    for (size_t i = 0; i < directives.size(); i++)
    {
        if (!directives[i].getDirective().compare("error_page"))
        {
            worker.setErrorPages(directives[i].getArgument());
            return;
        }
    }
}

template<typename T>
void    set(T& Directives,Worker &worker)
{
    for (size_t i = 0; i < Directives.size(); i++)
    {
        if (!Directives[i].getDirective().compare("root"))
        {
            worker.setRoot(Directives[i].getArgument()[0]);
            break;
        }
    }
}

template<typename T>
void    setDirectives(T &directives,Worker &worker)
{
    for (size_t i = 0; i < directives.size(); i++)
    {
        if (!directives[i].getDirective().compare("client_max_body_size"))
        {
            worker.set_max_body_size(directives[i].getArgument()[0]);
        }
        else if (!directives[i].getDirective().compare("autoindex"))
        {
            worker.setAutoIndex(directives[i].getArgument()[0]);
        }
        else if (!directives[i].getDirective().compare("to"))
        {
            worker.setRedirect(directives[i].getArgument()[0]);
        }
        else if (!directives[i].getDirective().compare("index"))
        {
            worker.setIndex(directives[i].getArgument(),worker.getRoot());
        }
    }
    
}

int Is_Directory(const std::string &root)
{
    struct stat fileInfo;

    if (stat(root.c_str(),&fileInfo) == 0)
    {
        if (S_ISDIR(fileInfo.st_mode))
        {
            return (0);
        }
        else if (S_ISREG(fileInfo.st_mode))
        {
            return (1);
        }
    }
    return (-1);
}

void get_query_string(std::string &path, std::string &query_string)
{
    size_t position = path.find('?');

    if (position != std::string::npos)
    {
        query_string = path.substr(position + 1);
        path = path.substr(0,position);
    }
}

void find_ip_address(const std::string &host,std::string &ipAddresses)
{
    
    struct addrinfo hints, *result, *p;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host.c_str(), NULL, &hints, &result);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        exit(1);
    }

    for (p = result; p != NULL; p = p->ai_next)
    {
        void* addr;
        char ipstr[INET_ADDRSTRLEN];

        if (p->ai_family == AF_INET) {
            struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(p->ai_addr);
            addr = &(ipv4->sin_addr);
        } 

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        ipAddresses += ipstr;
    }

    freeaddrinfo(result); 
}


void    get_matched_server_block(const std::string &host_name,std::vector<ServerBlocks> &blocks,Worker &worker,std::string &ip_address)
{
    for (size_t i = 0; i < blocks.size(); i++)
    {
        for (size_t j = 0; j < blocks[i].getDirectives().size() ; j++)
        {
            if (!blocks[i].getDirectives()[j].getDirective().compare("server_name"))
            {
                if (!blocks[i].getDirectives()[j].getArgument()[0].compare(host_name))
                {
                    worker.setBlockWorker(blocks[i]);
                    return;
                }
            }
        }
    }
    find_ip_address(host_name, ip_address);
}

void   init_worker_block(char buffer[1024],std::vector<ServerBlocks> &serverBlocks)
{
    int find;
    int is_dir = 0;
    int is_regular = 0;

    std::string host;
    std::string path;
    std::string method;
    std::string version;
    std::string mime_type;
    LocationsBlock locationworker;
    std::istringstream httpStream(buffer);
    std::string line;
    std::string hostname;
    std::string query_string;
    std::string ip_address;
    std::string port;
    
    Worker worker;
    

    httpStream >> method >> path >> version;
    get_query_string(path,query_string);
    std::getline(httpStream, line);
    while (std::getline(httpStream, line))
    {
        find = line.find(':');
        if (!line.substr(0,find).compare("Host"))
        {
            host = line.substr(find + 1);
        }
    }
    hostname = trim(host.substr(0,host.find(':'))).c_str();
    get_matched_server_block(hostname,serverBlocks,worker,ip_address);
    if (!ip_address.empty())
    {
        port = host.substr(host.find(':')).c_str();
        host = ip_address + port;
        worker = Worker(serverBlocks,host);
    }
    worker.setLocationWorker(worker.getBlockWorker(),host);
    set(worker.getLocationWorker().getDirectives(),worker);
    if (worker.getRoot().empty())
        set(worker.getBlockWorker().getDirectives(),worker);
    
    setDirectives(worker.getBlockWorker().getDirectives(),worker);
    setDirectives(worker.getLocationWorker().getDirectives(),worker);
    setAllowedmethods(worker,worker.getLocationWorker().getDirectives());

    if (worker.getAllowMethods().size() == 0)
        setAllowedmethods(worker,worker.getBlockWorker().getDirectives());
        
    setErrorPages(worker,worker.getLocationWorker().getDirectives());
    if (worker.getErrorPages().size() == 0)
        setErrorPages(worker,worker.getBlockWorker().getDirectives());

    if (!worker.getRoot().empty())
        worker.setRoot(worker.getRoot() + path);

    if (Is_Directory(worker.getRoot()) == 0 \
    || Is_Directory(worker.getRoot()) == 1 \
    || Is_Directory(worker.getRoot()) == -1)
    {
        if (Is_Directory(worker.getRoot()) == 0)
        {
            if (worker.getIndex().empty())
                worker.found_index_file(worker.getRoot());
            is_dir = 1;
        }
        else if (Is_Directory(worker.getRoot()) == 1)
            is_regular = 1;
    }
    if (is_regular == 1 || is_dir == 1)
    {
        printf("path %s =========================\n",path.c_str());
        printf("root %s\n",worker.getRoot().c_str());
        printf("body size %s\n",worker.get_max_body_size().c_str());
        printf("index %s\n",worker.getIndex().c_str());
        printf("auto index %s\n",worker.getAutoIndex().c_str());
        printf("methods allowed \t");
        
        for (size_t i = 0; i < worker.getAllowMethods().size(); i++)
        {
            printf("%s \t",worker.getAllowMethods()[i].c_str());
        }
        printf("\nerror pages \t");
        for (size_t i = 0; i < worker.getErrorPages().size(); i++)
        {
            printf("%s \t",worker.getErrorPages()[i].c_str());
        }
        printf("\n");
        printf("===============================================\n");
    }
}