/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blockworker.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lol <lol@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 15:21:48 by adardour          #+#    #+#             */
/*   Updated: 2024/01/14 17:30:15 by lol              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

std::string trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\n\r\f\v");

    if (start == std::string::npos) 
        return "";

    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

Worker::Worker(std::vector<ServerBlocks> &blocks,std::string &host)
{    
    for (size_t i = 0; i < blocks.size(); i++)
    {
        for (size_t j = 0; j < blocks[i].getDirectives().size(); j++)
        {
            if (!blocks[i].getDirectives()[j].getDirective().compare("listen"))
            {
                for (size_t k = 0; k < blocks[i].getDirectives()[j].getArgument().size(); k++)
                {
                    if (!trim(blocks[i].getDirectives()[j].getArgument()[0]).compare(trim(host)))
                    {
                        setBlockWorker(blocks[i]);
                        return;
                    }
                }
            }
        }
    }
}

void    Worker::setLocationWorker(const ServerBlocks& block,std::string &path)
{
   for (size_t i = 0; i < block.getLocations().size(); i++)
   {
        if (!block.getLocations()[i].getPath().compare(path))
        {
            this->locationworker = block.getLocations()[i];
        }
   }
}

void Worker::setIndex(const std::vector<std::string>&   args,const std::string &root)
{
    printf("root %s\n",root.c_str());
    if (!root.empty())
    {
        for (size_t i = 0; i < args.size(); i++)
        {
            if (access((root + args[i]).c_str(),F_OK) == 0)
            {
                this->index = root + args[i];
                break;
            }
        }
        
    }
}


void Worker::setMethod(std::vector<std::string>  &args)
{
    for (size_t i = 0; i < args.size(); i++)
    {
        printf("%s\n",args[i].c_str());
        allow_methods.push_back(args[i]);
    }
}