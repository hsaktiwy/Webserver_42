/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 16:13:26 by aalami            #+#    #+#             */
/*   Updated: 2024/02/16 18:28:15 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgiResponse.hpp"

CgiResponse::CgiResponse() : scriptData(NULL)
{
    responseSent = false;
    isDataset = false;
    isEnvObjectSet = false;
    isErrorResponse = false;
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
        isErrorResponse = obj.isErrorResponse;
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
        // printf("from cgi resp copy\n");
        // constructScriptEnv();
    }
    return (*this);
}

void CgiResponse::setSocket(int fd)
{
    socket_fd = fd;
}
void CgiResponse::creatCgiResponse()
{
    if (!Env.getStatus() && is)
    {
        std::string path_bin = "/usr/local/bin/python3";
        char **args;
        int status;
        args = new char *[3];
        args[0] = (char *)path_bin.c_str();
        args[1] = (char *)Env.getCgiScriptName().c_str();
        args[2] = NULL;
        int pid = fork();
        if (pid == 0)
        {
            dup2(socket_fd, 1); 
            execve(Env.getCgiScriptName().c_str(), args, scriptData);
            close (STDIN_FILENO);
            close (STDOUT_FILENO);
        }
        else 
       {     waitpid(pid, &status, 0);
            exit (1);}
    }
}
void CgiResponse::setCgiEnvObject(CgiEnv &obj)
{
    Env = obj;
    isEnvObjectSet = true;
    // constructScriptEnv();
}
void CgiResponse::setErrorResponseState()
{
    if (Env.getErrorPage().size() && Env.getErrorPage().compare("valid request") || Env.getErrorPage().empty())
        isErrorResponse = true;
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
        isDataset = true;
    }
    
    for (size_t i = 0; scriptData[i] != NULL; i++)
    {
        printf("%s\n", scriptData[i]);
    }
    
}
void CgiResponse::handleError()
{
    if (Env.isAutoIndexReq())
    {
        if (Env.getErrorPage().size())
        {
            errorResponse += "HTTP/1.1 302 Found\r\n";
            errorResponse += "Location: ";
            errorResponse += Env.getErrorPage() + "\r\n";
            errorResponse += "Content-Type: text/html\r\n\r\n";
        }
        else
        {
            errorResponse += "HTTP/1.1 403 Forbidden\r\n";
            errorResponse += "Content-Type: text/html\r\n\r\n";
            errorResponse += "<!DOCTYPE html>\r\n";
            errorResponse += "<html lang=\"en\">\r\n";
            errorResponse += "<head>\r\n";
            errorResponse += "    <meta charset=\"UTF-8\">\r\n";
            errorResponse += "    <title>403 Forbidden</title>\r\n";
            errorResponse += "</head>\r\n";
            errorResponse += "<body>\r\n";
            errorResponse += "    <h1>403 Forbidden</h1>\r\n";
            errorResponse += "    <p>You don't have permission to access this resource.</p>\r\n";
            errorResponse += "</body>\r\n";
            errorResponse += "</html>";
        }
    }
}
bool CgiResponse::isResponseSent()
{
    return responseSent;
}
bool CgiResponse::isEnvset()
{
    return isDataset;
}
bool CgiResponse::isReqObjectset()
{
    return isEnvObjectSet;
}