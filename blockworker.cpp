/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blockworker.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 15:21:48 by adardour          #+#    #+#             */
/*   Updated: 2024/01/16 13:15:19 by adardour         ###   ########.fr       */
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

bool prefix(const std::string &prefix,const std::string &path)
{   
    return (path.substr(0, prefix.length())).compare(prefix) == 0;
}
bool Worker::exact_match(const ServerBlocks &block,const std::string &path)
{
    for (size_t i = 0; i < block.getLocations().size(); i++)
    {
        if (!block.getLocations()[i].getPath().compare(path))
        {
            this->locationworker = block.getLocations()[i];
            return true;
        }
    }
    return false;
}
bool Worker::prefix_match(const ServerBlocks &block,const std::string &path)
{
    for (size_t i = 0; i < block.getLocations().size(); i++)
    {
        if (prefix(block.getLocations()[i].getPath(),path))
        {
            this->locationworker = block.getLocations()[i];
            return true;
        }
    }
    return false;
}

bool Worker::find_root(const ServerBlocks &block,const std::string &path)
{
    for (size_t i = 0; i < block.getLocations().size(); i++)
    {
        if (block.getLocations()[i].getPath().compare("/") == 0)
        {
            this->locationworker = block.getLocations()[i];
            return true;
        }
    }
    return false;
}

void    Worker::setLocationWorker(const ServerBlocks& block,std::string &path)
{
    if (exact_match(block,path))
        return;
    else if (prefix_match(block,path))
        return ;
    else if (find_root(block,path))
        return ;
}

void Worker::setIndex(const std::vector<std::string>&   args,const std::string &root)
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


void Worker::setMethod(std::vector<std::string>  &args)
{
    for (size_t i = 0; i < args.size(); i++)
    {
        allow_methods.push_back(args[i]);
    }
}

void    Worker::setErrorPages(std::vector<std::string>  &args)
{
    for (size_t i = 0; i < args.size(); i++)
    {
        this->error_page.push_back(args[i]);
    }
}