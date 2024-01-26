/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 16:49:20 by aalami            #+#    #+#             */
/*   Updated: 2024/01/26 20:46:08 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"

CgiEnv::CgiEnv(const Worker &workerObj) : worker(workerObj), cgiMetaData(NULL) 
{
}

void CgiEnv::setCgiServerName()
{
    std::string host;
    std::stringstream stream(worker.getHost());
    std::cout<<worker.getHost()<<std::endl;
    if (getline(stream, host, ':'))
        envMap["SERVER_NAME"] = host;
    else
        throw "Error while getting cgi meta data";
}
void CgiEnv::setCgiServePort()
{
    std::string port; 
    size_t index = worker.getHost().find(':');
    port = worker.getHost().substr(index + 1, worker.getHost().size() - index);
    envMap["SERVER_PORT"] = port;
}
void CgiEnv::setCgiQueryString()
{
    envMap["QUERY_STRING"] = worker.getQuery();
}

// void cgiEnv::set
std::string &CgiEnv::getCgiServerName() 
{
    return envMap["SERVER_NAME"];
}
std::string &CgiEnv::getCgiServerPort()
{
    return envMap["SERVER_PORT"];
}
std::string &CgiEnv::getCgiQueryString()
{
    return envMap["QUERY_STRING"];
}
