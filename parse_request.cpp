/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 19:33:06 by adardour          #+#    #+#             */
/*   Updated: 2024/01/13 21:53:05 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

template<typename T>
void    set(Worker &worker, T& Directives,int flag)
{
    std::string directive;

    directive = (flag == 0) ? "root" : "index";
    for (size_t i = 0; i < Directives.size(); i++)
    {
        if (!Directives[i].getDirective().compare(directive))
        {
            if (flag == 0)
            {
                
                worker.setRoot(Directives[i].getArgument()[0]);
                break;
            }
            else if (flag == 1)
            {
                for (size_t j = 0; j < Directives[i].getArgument().size(); j++)
                {
                    if (access((worker.getRoot() + Directives[i].getArgument()[j]).c_str(),F_OK) == 0)
                    {
                        worker.setIndex(worker.getRoot() + Directives[i].getArgument()[j]);
                        break;
                    }
                }
                
            }
        }
        }
    // worker.setRoot("");
}

void    build_response(Worker &worker,std::string &response)
{
    std::ifstream myfile;
    myfile.open(worker.getIndex());
    std::string myline;
    if ( myfile.is_open() )
    {
        while ( myfile )
        {
            std::getline (myfile, myline);
            response += myline;
        }
        myfile.close();
    }
    else {
        std::cout << "Couldn't open file\n";
    }
}

std::string    is_mime_type(std::string &path)
{
    int length = path.length();
    while (length)
    {
        if (path[length] == '.')
        {
            break;
        }
        length--;
    }
    return (path.substr(length + 1));
}

void    response_mime_type(std::string &response, std::string &mime_type, int *status)
{
    int length = response.length();
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/" + mime_type + "\nContent-Length: " + std::to_string(length) + "\n\n" + response;
    printf("response %s\n",response.c_str());
    *status = 1;
}

std::string&    parse_request(char buffer[1024],std::vector<ServerBlocks> &serverBlocks,std::string &response,int *flag,int *status)
{
    int find;

    std::string host;
    std::string path;
    std::string method;
    std::string version;
    std::string mime_type;

    std::istringstream httpStream(buffer);
    std::string line;

    static Worker worker;
    httpStream >> method >> path >> version;
    mime_type = is_mime_type(path);
    if (mime_type.compare("css"))
    {
        std::getline(httpStream, line);
        while (std::getline(httpStream, line))
        {
            find = line.find(':');
            if (!line.substr(0,find).compare("Host"))
            {
                host = line.substr(find + 1);
            }
        }
        worker = Worker(serverBlocks,host);
        worker.setLocationWorker(worker.getBlockWorker(),path);
        set(worker,worker.getLocationWorker().getDirectives(),0);
        if (worker.getRoot().empty())
            set(worker,worker.getBlockWorker().getDirectives(),0);
        if (!worker.getRoot().empty())
        {
            set(worker,worker.getLocationWorker().getDirectives(),1);
            if (worker.getIndex().empty())
                set(worker,worker.getBlockWorker().getDirectives(),1);
            if (!worker.getIndex().empty())
            {
                build_response(worker, response);
                *flag = 0;
            }
            else 
            {
                // printf("404 not found\n");
            }
        }
        else
        {
            // printf("404 not found\n");
        }
    }
    else
    {

        std::ifstream myfile;
        std::string full_path = worker.getRoot() + path.substr(path.find('/') + 1);
        myfile.open(full_path);
        std::string myline;
        if ( myfile.is_open() )
        {
            while ( myfile )
            {
                std::getline (myfile, myline);
                response += myline;
            }
            response_mime_type(response,mime_type,status);
            myfile.close();
        }
        else {
            std::cout << "Couldn't open file\n";
        }
    }
    return response;
}