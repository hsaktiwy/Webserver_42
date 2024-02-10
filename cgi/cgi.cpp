/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 16:49:20 by aalami            #+#    #+#             */
/*   Updated: 2024/02/10 17:05:50 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"

CgiEnv::CgiEnv(const Worker &workerObj) : worker(workerObj), cgiMetaData(NULL) 
{
    validRoot = true;
    cgiDir = false;
    cgiScript = false;
    extrapath = false;
    autoIndex =  false;
    isDir = false;
    isFile = false;
    status = 0;
    extraPathIndex = -1;
    setPathUriVector();
    setCgiRoot();
    findScript();
    setCgiPATHINFO();
    setCgiQueryString();
    setCgiServerName();
    setCgiServePort();
    setRequestMethod();
    constructScriptEnv();
    std::cout<<RED<<envMap["SCRIPT_NAME"]<<" "<<status <<RESET<<std::endl;
    std::cout<<RED<<envMap["PATH_INFO"]<<" "<<status <<RESET<<std::endl;
}
void CgiEnv::setRequestMethod()
{
    envMap["REQUEST_METHOD"]="GET";
}
void CgiEnv::setPathUriVector()
{
    std::string path = worker.getPath();
    std::string value;
    size_t delimIndex = path.find('/', 0);
    while (delimIndex != std::string::npos)
    {
        size_t i = delimIndex + 1;
        while(path[i] == '/' && i < path.size())
            i++;
        while(path[i] != '/' && i < path.size())
        {
            value.push_back(path[i]);
            i++;
        }
        if (!value.empty())
        {
            pathUri.push_back(value);
            value.clear();
        }
        if (i >= path.size())
            break;
        delimIndex = path.find('/', i);
    }
    for (size_t i = 0; pathUri.size() > i; i++)
        printf("%s\n", pathUri[i].c_str());
    
}
void CgiEnv::setCgiRoot() //this function check if the root directive set for the appropriate cgi request have a "cgi-bin" subdirectory 
{
    std::string cgiDirecitory = "cgi-bin";
    DIR *rootPath = opendir(worker.getRoot().c_str());
    int isValidDir = 0;
    int accessDir = 0;

    if (rootPath == NULL)
    {
        validRoot = false ;
        if (errno == EACCES)
            status = 403;
        else
            status = 404;
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
            cgiRoot = root + cgiDirecitory ;
            accessDir = access(cgiRoot.c_str(), F_OK | X_OK);
            isValidDir = Is_Directory(cgiRoot);
            if (!isValidDir && !accessDir)
                cgiDir = true;
            else if (isValidDir == -1 || accessDir == -1)
            {
                if (accessDir == -1)
                    status = 403;
                else
                    status = 404;
            }
            std::cout<<GREEN<<isValidDir<< " sss =>>"<<cgiRoot<<RESET<<std::endl;
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
    std::string currentDir = cgiRoot;
    std::string extraPath;

    extraPath += cgiRoot;
    if (extraPathIndex != -1)
    {
        if (validRoot && cgiDir && cgiScript)
        {
            for (size_t i = extraPathIndex; i < pathUri.size(); i++)
            {
                extraPath += "/"; 
                extraPath += pathUri[i];
            }
                printf("hhhhhhh    %s\n", extraPath.c_str());
            if (!access(extraPath.c_str(), F_OK | R_OK | X_OK))
            {
                envMap["PATH_INFO"] = extraPath;
                extraPath = true;
            }
            else
                errno == EACCES ? status = 403 : status = 404;
        }
    }
}

void CgiEnv::findScript()
{
    DIR *cgiDir;
    std::string currentDir = cgiRoot;
    int is_file = 0;
    bool next = false;
    
    if (validRoot && cgiDir)
    {
        for (size_t i = 0; i < pathUri.size() - 1; i++)
        {
            is_file = Is_Directory(currentDir);
            if(!is_file)
            {
                cgiDir = opendir(currentDir.c_str());
                if (cgiDir == NULL)
                {
                    errno == EACCES ? status = 403 : status = 404;
                    return;
                }
                struct dirent *entry = readdir(cgiDir);
                std::string entryString;
                while (entry)
                {
                        printf("%s %s\n", entry->d_name,pathUri[i + 1].c_str());
                    entryString = entry->d_name;
                    if (!entryString.compare(pathUri[i + 1]))
                    {
                        next =  true;
                        is_file =  Is_Directory(currentDir  + "/" + pathUri[i + 1]);
                        if (is_file)
                        {
                            if (!access((currentDir  + "/" + pathUri[i + 1]).c_str(), F_OK | R_OK | X_OK))
                            {
                                cgiScript = true;
                                envMap["SCRIPT_NAME"] = currentDir  + "/" + pathUri[i + 1];
                                if (i + 2 < pathUri.size())
                                    extraPathIndex = i + 2;
                            }
                            else
                                errno == EACCES ? status = 403 : status = 404;
                            closedir(cgiDir);
                            return;
                        }
                        else if (!is_file)
                            currentDir = currentDir  + "/" + pathUri[i + 1];
                        break;
                    }
                    entry = readdir(cgiDir);
                }
                closedir(cgiDir);
            }
            else if (is_file == 1)
            {
                if (!access((currentDir  + "/" + pathUri[i + 1]).c_str(), F_OK | R_OK | X_OK))
                {
                    cgiScript = true;
                    envMap["SCRIPT_NAME"] = currentDir  + "/" + pathUri[i + 1];
                    if (i + 2 < pathUri.size())
                        extraPathIndex = i + 2;
                }
                else
                    errno == EACCES ? status = 403 : status = 404;
                return;
            }
            else
            {
                status = 404;
                return;
            }
        }
    }
    if (!cgiScript && !Is_Directory(currentDir))
    {
        cgiDir = opendir(currentDir.c_str());
        if (cgiDir == NULL)
        {
            errno == EACCES ? status = 403 : status = 404;
            return;
        }
        if (worker.getIndex().size())
        {
            struct dirent *entry = readdir(cgiDir);
            std::string entryString;
            while(entry)
            {
                entryString = entry->d_name;
                if(!entryString.compare(worker.getIndex()))
                {
                    if (!access((currentDir  + "/" + entryString).c_str(), F_OK | R_OK | X_OK))
                    {
                        cgiScript = true;
                        envMap["SCRIPT_NAME"] = currentDir  + "/" + entryString;
                    }
                    else
                        errno == EACCES ? status = 403 : status = 404;
                    closedir(cgiDir);
                    return;
                }
                entry = readdir(cgiDir);
            }
            closedir(cgiDir);
        }
        if (!cgiScript && worker.getAutoIndex().size() && !worker.getAutoIndex().compare("on"))
            autoIndex = true;
        else
            status = 404;
    }
}

void CgiEnv::constructScriptEnv()
{
    size_t env_size = envMap.size();
    std::map<std::string, std::string>::iterator it;
    std::string data;
    int count = 0;
    if (env_size)
    {
        cgiMetaData = new char* [env_size + 1];
        for (it = envMap.begin(); it != envMap.end(); it++)
        {
            data = it->first + "=" + it->second;
            cgiMetaData[count] = strdup(data.c_str());
            count++;
        }
        cgiMetaData[count] = NULL;
    }
    for (size_t i = 0; cgiMetaData[i] != NULL; i++)
    {
        printf("%s\n", cgiMetaData[i]);
    }
    
}
// bool CgiEnv::isDirectoryRequest()
// {
//     DIR *cgiDir;
//     std::string currentDir = cgiRoot;
    

//     if (validRoot && cgiDir)
//     {
//         for (size_t i = 1; i < pathUri.size() - 1; i++) // we started from 1 because the first element in vector is the cgi-bin dir
//         {            
//             cgiDir = opendir(currentDir.c_str());
//             if (cgiDir != NULL)
//             {
//                 struct dirent *entry = readdir(cgiDir);
//                 while (readdir)
//                 {
//                     if(!pathUri[i + 1].compare(entry->d_name))
//                     {
//                         if(access((cgiRoot )))
//                     }
//                 }
//             }
            
//         }
        
//     }
   
// }
// void CgiEnv::setCgiScriptName()
// {
//     std::string root;
//     std::string path;
//     std::string fullPath; // include extra infos
//     std::string cgiRoot;
//     cgiRoot = getCgiRoot();
//     if (validRoot && cgiDir)
//     {
//         if(!worker.getPath().compare("/cgi-bin") || !worker.getPath().compare("/cgi-bin/"))
//         {
//             isDir = true;
//             if (worker.getIndex().size())
//             {
//                 if (access((cgiRoot + worker.getIndex()).c_str(), R_OK | X_OK | F_OK))
//                     envMap["SCRIPT_NAME"] = worker.getIndex();
//                 else
//                     autoIndex = true;
//             }
//         }
//         else
        
//     }
//     std::stringstream stream;
//     root = worker.getRoot();
//     if (root[root.size() - 1] == '/')
//         root.pop_back();
//     path = worker.getPath().substr(1, worker.getPath().size() - 1);
//     path =
//     fullPath = root + path;
    
// }
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
std::string &CgiEnv::getCgiScriptName()
{
    return envMap["SCRIPT_NAME"];
}
std::string &CgiEnv::getCgiRoot()
{
    return(cgiRoot);
}
bool CgiEnv::getStatus()
{
    return status;
}
bool CgiEnv::getCgiDirStatus()
{
    return cgiDir;
}
bool CgiEnv::isScriptFound()
{
    return cgiScript;
}
char **CgiEnv::getenvArray()
{
    return cgiMetaData;
}