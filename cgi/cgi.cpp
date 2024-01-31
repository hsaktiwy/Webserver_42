/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 16:49:20 by aalami            #+#    #+#             */
/*   Updated: 2024/01/31 22:36:20 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"

CgiEnv::CgiEnv(const Worker &workerObj) : worker(workerObj), cgiMetaData(NULL) 
{
    validRoot = true;
    cgiDir = false;
}

void CgiEnv::setCgiRoot() //this function check if the root directive set for the appropriate cgi request have a "cgi-bin" subdirectory 
{
    std::string cgiDirecitory = "cgi-bin";
    DIR *rootPath = opendir(worker.getRoot().c_str());

    if (rootPath != NULL)
    {
        validRoot = false ;
        return;
    }
    struct  dirent *entry = readdir(rootPath);
    while(entry)
    {
        if (!cgiDirecitory.compare(entry->d_name))
        {
            std::string root = worker.getRoot();
            if (root[root.size() - 1] != '/')
                root.push_back('/');
            cgiRoot = root + cgiDirecitory;
            cgiDir = true;
            break;
        }
        entry = readdir(rootPath);
    }    
    closedir(rootPath);
}
void CgiEnv::setCgiServerName()
{
    std::string host;
    std::stringstream stream(worker.getHost());
    
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

void CgiEnv::setCgiPATHINFO()
{
    std::string root;
    std::string path;
    root =  worker.getRoot();
    if (root[root.size() - 1] == '/')
        root.pop_back();
    path = worker.getPath();
    path = root + path; // full path includes the extra info (PATH_INFO)
    envMap["PATH_INFO"] = path;
}

void CgiEnv::setCgiScriptPath()
{
    std::string root;
    std::string path;
    std::string fullPath; // include extra infos
    std::stringstream stream;
    root = worker.getRoot();
    if (root[root.size() - 1] == '/')
        root.pop_back();
    path = worker.getPath();
    fullPath = root + path;
    
}
std::string &CgiEnv::getCgiPATHINFO()
{
    return envMap["PATH_INFO"];
}
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
std::string &CgiEnv::getCgiRoot()
{
    return(cgiRoot);
}
