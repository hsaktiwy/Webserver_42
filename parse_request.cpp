/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 19:33:06 by adardour          #+#    #+#             */
/*   Updated: 2024/01/14 12:39:03 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

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
    *status = 1;
}

bool is_directive(const std::string &directive)
{
    std::vector<std::string> tokenTypes;
    
    tokenTypes.push_back("client_max_body_size");
    tokenTypes.push_back("error_page");
    tokenTypes.push_back("autoindex");
    tokenTypes.push_back("allow_methods");
    tokenTypes.push_back("index");
    tokenTypes.push_back("access_log");
    tokenTypes.push_back("error_log");
    tokenTypes.push_back("root");
    tokenTypes.push_back("to");
    tokenTypes.push_back("cgi");
    
    if (std::find(tokenTypes.begin(), tokenTypes.end(), directive) != tokenTypes.end())
    {
        return true;
    }
    return false;
}

void    setDirectives(Worker &worker)
{
    set(worker.getLocationWorker().getDirectives(),worker);
    if (worker.getRoot().empty())
        set(worker.getBlockWorker().getDirectives(),worker);
    
    int i = 0;
    while (i < worker.getLocationWorker().getDirectives().size())
    {
        if (is_directive(worker.getLocationWorker().getDirectives()[i].getDirective()))
        {
            for (size_t j = 0; j < worker.getLocationWorker().getDirectives()[i].getArgument().size(); j++)
            {
                if (!worker.getLocationWorker().getDirectives()[i].getDirective().compare("index"))
                {
                    worker.setIndex(worker.getLocationWorker().getDirectives()[i].getArgument(),worker.getRoot());
                }
                else if (!worker.getLocationWorker().getDirectives()[i].getDirective().compare("autoindex"))
                {
                    worker.setAutoIndex(worker.getLocationWorker().getDirectives()[i].getArgument()[0]);
                }
                else if (!worker.getLocationWorker().getDirectives()[i].getDirective().compare("to"))
                {
                    worker.setRedirect(worker.getLocationWorker().getDirectives()[i].getArgument()[0]);
                }
                else if (!worker.getLocationWorker().getDirectives()[i].getDirective().compare("client_max_body_size"))
                {
                    worker.set_max_body_size(worker.getLocationWorker().getDirectives()[i].getArgument()[0]);
                }
                else if (!worker.getLocationWorker().getDirectives()[i].getDirective().compare("allow_methods"))
                {
                    worker.setMethod(worker.getLocationWorker().getDirectives()[i].getArgument());
                }
            }
        }
        else
        {
            for (size_t j = 0; j < worker.getBlockWorker().getDirectives()[i].getArgument().size(); j++)
            {
                if (!worker.getBlockWorker().getDirectives()[i].getDirective().compare("index"))
                {
                    worker.setIndex(worker.getBlockWorker().getDirectives()[i].getArgument(),worker.getRoot());
                }
                else if (!worker.getBlockWorker().getDirectives()[i].getDirective().compare("autoindex"))
                {
                    worker.setAutoIndex(worker.getBlockWorker().getDirectives()[i].getArgument()[0]);
                }
                else if (!worker.getBlockWorker().getDirectives()[i].getDirective().compare("to"))
                {
                    worker.setRedirect(worker.getBlockWorker().getDirectives()[i].getArgument()[0]);
                }
                else if (!worker.getBlockWorker().getDirectives()[i].getDirective().compare("client_max_body_size"))
                {
                    worker.set_max_body_size(worker.getBlockWorker().getDirectives()[i].getArgument()[0]);
                }
                else if (!worker.getBlockWorker().getDirectives()[i].getDirective().compare("allow_methods"))
                {
                    worker.setMethod(worker.getBlockWorker().getDirectives()[i].getArgument());
                }
            }
        }
        i++;
    }    
}

std::string&    parse_request(char buffer[1024],std::vector<ServerBlocks> &serverBlocks,std::string &response,int *flag,int *status,std::string &human_status)
{
    int find;

    std::string host;
    std::string path;
    std::string method;
    std::string version;
    std::string mime_type;

    std::istringstream httpStream(buffer);
    std::string line;

    ;
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
        Worker worker = Worker(serverBlocks,host);
        worker.setLocationWorker(worker.getBlockWorker(), path);
        setDirectives(worker);

        printf("index = %s\n",worker.getIndex().c_str());
        printf("auto index = %s\n",worker.getAutoIndex().c_str());
        printf("redirect = %s\n",worker.getRedirect().c_str());
        printf("max body size = %s\n",worker.get_max_body_size().c_str());

        // for (size_t i = 0; i < worker.getAllowMethods().size(); i++)
        // {
        //     printf("%s\n",worker.getAllowMethods()[i].c_str());
        // }
        
        
    }
    // else
    // {
    //     std::ifstream myfile;
    //     std::string full_path = worker.getRoot() + path.substr(path.find('/') + 1);
    //     myfile.open(full_path);
    //     std::string myline;
    //     if ( myfile.is_open() )
    //     {
    //         while ( myfile )
    //         {
    //             std::getline (myfile, myline);
    //             response += myline;
    //         }
    //         response_mime_type(response,mime_type,status);
    //         myfile.close();
    //     }
    //     else {
    //         std::cout << "Couldn't open file\n";
    //     }
    // }
    return response;
}