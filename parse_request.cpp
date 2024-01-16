/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 19:33:06 by adardour          #+#    #+#             */
/*   Updated: 2024/01/16 13:17:31 by adardour         ###   ########.fr       */
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
template<typename T>
void    setDirectives(T &directives,Worker &worker,int flag)
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

std::string&    parse_request(char buffer[1024],std::vector<ServerBlocks> \
&serverBlocks,std::string &response,int *flag,int *status,std::string &human_status)
{
    int find;

    std::string host;
    std::string path;
    std::string method;
    std::string version;
    std::string mime_type;
    LocationsBlock locationworker;
    std::istringstream httpStream(buffer);
    std::string line;
    Worker worker;
    

    httpStream >> method >> path >> version;
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
    set(worker.getLocationWorker().getDirectives(),worker);
    if (worker.getRoot().empty())
    {
        set(worker.getBlockWorker().getDirectives(),worker);
    }
    setDirectives(worker.getBlockWorker().getDirectives(),worker,0);
    setDirectives(worker.getLocationWorker().getDirectives(),worker,1);
    setAllowedmethods(worker,worker.getLocationWorker().getDirectives());
    if (worker.getAllowMethods().size() == 0)
        setAllowedmethods(worker,worker.getBlockWorker().getDirectives());
    setErrorPages(worker,worker.getLocationWorker().getDirectives());
    if (worker.getErrorPages().size() == 0)
        setErrorPages(worker,worker.getBlockWorker().getDirectives());
    if (!worker.getRoot().empty())
    {
        worker.setRoot(worker.getRoot() + path);
    }
    // worker.setPath(worker.getRoot() + worker.getPath());


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
    

    response += "Hello";
    human_status = "OK";
    *status = 200; 
    return response;
}