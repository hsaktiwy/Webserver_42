/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_worker_block.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 19:33:06 by adardour          #+#    #+#             */
/*   Updated: 2024/01/26 19:05:36 by aalami           ###   ########.fr       */
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
        }
    }
}

template<typename T>
void    set(T& Directives,Worker &worker, const std::string &path)
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
                worker.setIndex(directives[i].getArgument(),worker.getRoot());//, worker.get);
        }
        // else if (!directives[i].getDirective().compare("index"))
        // {
        //         worker.setIndex(directives[i].getArgument(),worker.getRoot());//, worker.get);
        // }
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

void find_ip_address(const std::string &host,std::string &ipAddresses)
{
    
    struct addrinfo hints, *result, *p;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host.c_str(), NULL, &hints, &result);
    
    if (status != 0)
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        exit(1);
    }
    for (p = result; p != NULL; p = p->ai_next)
    {
        void* addr;
        char ipstr[INET_ADDRSTRLEN];

        if (p->ai_family == AF_INET)
        {
            struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(p->ai_addr);
            addr = &(ipv4->sin_addr);
        } 
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        ipAddresses += ipstr;
        break;
    }

    freeaddrinfo(result); 
}


void    get_matched_server_block(const std::string &host_name,std::vector<ServerBlocks> &blocks,Worker &worker,std::string &ip_address)
{
    for (size_t i = 0; i < blocks.size(); i++)
    {
        for (size_t j = 0; j < blocks[i].getDirectives().size() ; j++)
        {
            if (!blocks[i].getDirectives()[j].getDirective().compare("server_names"))
            {
                for (size_t k = 0; k < blocks[i].getDirectives()[j].getArgument().size(); k++)
                {
                    if (!blocks[i].getDirectives()[j].getArgument()[k].compare(host_name))
                    {
                        worker.setBlockWorker(blocks[i]);
                        return;
                    }
                }
            }
        }
    }
    find_ip_address(host_name, ip_address);
}

void    set_based_ip_address(Worker &worker, std::vector<ServerBlocks> &serverBlocks, const std::string listen)
{
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        for (size_t j = 0; j < serverBlocks[i].getDirectives().size() ; j++)
        {
            if (!serverBlocks[i].getDirectives()[j].getDirective().compare("listen"))
            {
                for (size_t k = 0; k < serverBlocks[i].getDirectives()[j].getArgument().size(); k++)
                {
                    if (!serverBlocks[i].getDirectives()[j].getArgument()[k].compare(listen))
                    {
                        worker.setBlockWorker(serverBlocks[i]);
                        return;
                    }
                }
            }
        }
    }
}

void   init_worker_block(Worker &worker, std::string &host, std::string &path,std::vector<ServerBlocks> &serverBlocks, int &is_dir, int &is_regular)
{

    int find;

    std::string hostname;
    std::string ip_address;
    std::string port;

    worker.setHost(host);
    worker.setPath(path);
    hostname = trim(host.substr(0,host.find(':'))).c_str();
    get_matched_server_block(hostname,serverBlocks,worker,ip_address);
    if (!ip_address.empty())
    {
        port = host.substr(host.find(':')).c_str();
        host = ip_address + port;
    set_based_ip_address(worker,serverBlocks,host);
    }
    worker.setLocationWorker(worker.getBlockWorker(),path);
    set(worker.getLocationWorker().getDirectives(),worker,worker.getPath());
    if (worker.getRoot().empty())
        set(worker.getBlockWorker().getDirectives(),worker,worker.getPath());
    // int flag = 0;
    setDirectives(worker.getBlockWorker().getDirectives(),worker);
    // if (worker.getIndex().empty())
    //     flag = 1;
    setDirectives(worker.getLocationWorker().getDirectives(),worker);
    // if (flag == 1 && !worker.getIndex().empty())
    //     worker.setIndex2(((std::string &)worker.getLocationWorker().getPath()) + "/" + worker.getIndex());
    setAllowedmethods(worker,worker.getLocationWorker().getDirectives());

    if (worker.getAllowMethods().size() == 0)
        setAllowedmethods(worker,worker.getBlockWorker().getDirectives());
        
    setErrorPages(worker,worker.getLocationWorker().getDirectives());
    if (worker.getErrorPages().size() == 0)
        setErrorPages(worker,worker.getBlockWorker().getDirectives());
    std::string tmp_path = worker.getRoot() + path;
    if (Is_Directory(tmp_path) == 0 \
    || Is_Directory(tmp_path) == 1 \
    || Is_Directory(tmp_path) == -1)
    {
        if (Is_Directory(tmp_path) == 0)
        {
            if (worker.getIndex().empty())
                worker.found_index_file(tmp_path);
            is_dir = 1;
        }
        else if (Is_Directory(tmp_path) == 1)
            is_regular = 1;
    }
    // if (is_regular)
    // {
    //     printf("server this %s\n",path.c_str());
    // }
    // printf("path %s\n",worker.getPath().c_str());
    // printf("size %lu\n",(worker.getErrorPages().size()));
}