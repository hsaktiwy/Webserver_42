/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 19:33:06 by adardour          #+#    #+#             */
/*   Updated: 2024/01/10 19:33:18 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

std::string trim(const std::string& str)
{
    
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}


ServerBlocks get_server_block(std::string &host,std::vector<ServerBlocks> &serverBlocks)
{
    int i = 0;
    ServerBlocks block;
    while (i < serverBlocks.size())
    {
        int j = 0;
        while (j < serverBlocks[i].getDirectives().size())
        {
            if (!serverBlocks[i].getDirectives()[j].getDirective().compare("listen") && !serverBlocks[i].getDirectives()[j].getArgument()[0].compare(trim(host).c_str()))
            {
                block = serverBlocks[i];
            }
            j++;
        }
        
        i++;
    }
    
    return block;
}
std::string get_index(std::vector<Directives> &directives)
{
    for (size_t i = 0; i < directives.size(); i++)
    {
        if (!directives[i].getDirective().compare("index"))
        {
            for (size_t j = 0; j < directives[j].getArgument().size(); j++)
            {
                return (directives[i].getArgument()[j]);
                break;
            }
            
        }
    }
    
    return "NULL";
}

std::string get_root(std::vector<Directives> &directives)
{
    for (size_t i = 0; i < directives.size(); i++)
    {
        if (!directives[i].getDirective().compare("root"))
        {
            for (size_t j = 0; j < directives[i].getArgument().size(); j++)
            {
                return directives[i].getArgument()[0];
            }
        }
    }
    return ("NULL");
}

std::string check_root(ServerBlocks &block)
{
    std::string root;
    for (size_t i = 0; i < block.getDirectives().size(); i++)
    {
        root = get_root(block.getDirectives());
    }
    return root;
}

std::string&    parse_request(char buffer[1024],std::vector<ServerBlocks> &serverBlocks,std::string &response)
{
    std::string method;
    std::string path;   
    std::string host;   
    std::map<std::string,std::string> headers;
    int find;
    std::string key;
    std::string value;
    std::string first;
    std::string second;
    std::string index_path;
    std::string root;
    std::string serve;
    ServerBlocks block;

    
    std::istringstream iss(buffer);
    std::string line;
    if (std::getline(iss, line)) {
        std::istringstream lineStream(line);
        lineStream >> method >> path; 
    }
    while (std::getline(iss, line))
    {
       find = line.find(':');
       key = line.substr(0,find);
       value = line.substr(find + 1);
       headers.insert(std::make_pair(key,value));
    }

    std::map<std::string,std::string>::iterator it = headers.begin();
    std::map<std::string,std::string>::iterator ite = headers.end();

    while (it != ite)
    {
        first = it->first;
        second = it->second;
        if (!first.compare("Host"))
        {
            host = second;
            break;
        }
        it++;
    }
    
    // std::cout << host << std::endl;
    // std::cout << method << std::endl;
    // std::cout << path << std::endl;

    if (!path.compare("/") || !path.compare("/index.html") || !path.compare("/index.php"))
    {
        block = get_server_block(host,serverBlocks);
        for (size_t i = 0; i < block.getDirectives().size(); i++)
        {
            index_path = get_index(block.getDirectives());
            break;
        }
        if (index_path.compare("NULL"))
        {
            root = check_root(block);
            if (root.compare("NULL"))
            {
                serve = root + "/" + index_path;
                std::ifstream myfile;
                myfile.open(serve);
                std::string myline;
                if (myfile.is_open())
                {
                    while ( myfile )
                    { 
                        std::getline (myfile, myline);
                        response += myline;
                    }
                    myfile.close();
                }
                else
                {
                    std::cout << "Couldn't open file\n";
                }
            }
            else
            {
                printf("list 1\n");
            }
        }
        else
        {
            printf("list 2\n");
        }
    }

    return response;
}