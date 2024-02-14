/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 16:13:26 by aalami            #+#    #+#             */
/*   Updated: 2024/02/14 22:02:32 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgiResponse.hpp"

CgiResponse::CgiResponse() : scriptData(NULL)
{
    responseSent = false;
    socket_fd = -1;
}
CgiResponse::~CgiResponse()
{
    if (scriptData != NULL)
    {
        size_t size = Env.getEnvMap().size();
        for (size_t i = 0; i < size; i++)
            delete [] scriptData[i];
        delete [] scriptData[size];
        delete [] scriptData;
        scriptData = NULL;
    }
}
CgiResponse::CgiResponse(const CgiResponse &obj)
{
    *this = obj;
}
CgiResponse &CgiResponse::operator=(const CgiResponse &obj)
{
    if (this != &obj)
    {
        responseSent = obj.responseSent;
        socket_fd = obj.socket_fd;
        if (scriptData != NULL)
        {
            size_t size = Env.getEnvMap().size();
            for (size_t i = 0; i < size; i++)
                delete [] scriptData[i];
            delete [] scriptData[size];
            delete [] scriptData;
            scriptData = NULL;
        }
        Env = obj.Env;
        constructScriptEnv();
    }
    return (*this);
}
// void cgiResponse::creatCgiResponse()
// {
//     if (!Env.getStatus())
//     {
//         char **
//         std::string path_bin = "/usr/local/bin/python3";
//         char **args;
//         args = new char *[3];
//         args[0] = (char *)path_bin.c_str();
//         args[1] = (char *)Env.getCgiScriptName().c_str();
//         args[2] = NULL;
//         int pid = fork();
//         if (pid == 0)
//         {
//             execve(Env.getCgiScriptName().c_str(), args, Env.getenvArray());
//         }
//     }
// }
void CgiResponse::setCgiEnvObject(CgiEnv &obj)
{
    Env = obj;
    constructScriptEnv();
}
void CgiResponse::constructScriptEnv()
{
    std::map<std::string, std::string> envMap = Env.getEnvMap();
    size_t env_size = envMap.size();
    std::map<std::string, std::string>::iterator it;
    std::string data;
    int count = 0;
    if (env_size)
    {
        scriptData = new char* [env_size + 1];
        for (it = envMap.begin(); it != envMap.end(); it++)
        {
            data = it->first + "=" + it->second;
            scriptData[count] = strdup(data.c_str());
            count++;
        }
        scriptData[count] = NULL;
    }
    for (size_t i = 0; scriptData[i] != NULL; i++)
    {
        printf("%s\n", scriptData[i]);
    }
    
}
bool CgiResponse::isResponseSent()
{
    return responseSent;
}