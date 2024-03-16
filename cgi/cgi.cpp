/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 16:49:20 by aalami            #+#    #+#             */
/*   Updated: 2024/03/16 05:36:30 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"

CgiEnv::CgiEnv(const Worker &workerObj) : worker(workerObj)
{
    validRoot = true;
    cgiDir = false;
    cgiScript = false;
    extrapath = false;
    autoIndex =  false;
    isDir = false;
    isFile = false;
    status = 0;
    isRedir = false;
    extraPathIndex = -1;
    // setPathUriVector();
    // setCgiRoot();
    // findScript();
    // setCgiPATHINFO();
    // setCgiQueryString();
    // setCgiServerName();
    // setCgiServePort();
    // setRequestMethod();
    // constructScriptEnv();
    // std::cout<<RED<<envMap["SCRIPT_NAME"]<<" "<<status <<RESET<<std::endl;
    // std::cout<<RED<<envMap["PATH_INFO"]<<" "<<status <<RESET<<std::endl;
}
CgiEnv::CgiEnv()
{
    ismetaDataset = false;
    validRoot = true;
    cgiDir = false;
    cgiScript = false;
    extrapath = false;
    autoIndex =  false;
    isDir = false;
    isFile = false;
    isRedir = false;
    status = 0;
    extraPathIndex = -1;
}
CgiEnv::CgiEnv(const CgiEnv &obj)
{
    *this = obj;
}
CgiEnv &CgiEnv::operator=(const CgiEnv &obj)
{
    if (this != &obj)
    {
        envMap = obj.envMap;
        pathUri = obj.pathUri;
        worker = obj.worker;
        cgiRoot = obj.cgiRoot;
        validRoot = obj.validRoot;
        cgiDir = obj.cgiDir;
        cgiScript = obj.cgiScript;
        extrapath = obj.extrapath;
        autoIndex = obj.autoIndex;
        isDir = obj.isDir;
        isFile = obj.isFile;
        status = obj.status;
        extraPathIndex = obj.extraPathIndex;
        errorPage = obj.errorPage;
        reqBody = obj.reqBody;
        boundary = obj.boundary;
        scriptBin = obj.scriptBin;
        redirection = obj.redirection;
        isRedir = obj.isRedir;
    }
    return (*this);
}

void CgiEnv::setRequestBody(const std::string &body)
{
    reqBody = body;
}
void CgiEnv::setContentType(std::string &value)
{
    envMap["CONTENT_TYPE"] = value;
}
void CgiEnv::setContentLength(std::string &value)
{
    envMap["CONTENT_LENGTH"] = value;
}

void CgiEnv::setCgiWorker(const Worker &obj)
{
    worker = obj;
}
void CgiEnv::processAndSetSessions(std::string &value)
{
    std::stringstream stream(value);
    std::string str;
    std::string key;
    std::string val;
    // std::getline(stream, str, ';');
    if (value.find(';') !=  std::string::npos)
    {
        while(getline(stream, str, ';'))
        {
            size_t delim = str.find('=');
            key = str.substr(0, delim);
            size_t not_space = key.find_first_not_of(' ');
            if (not_space != std::string::npos)
                key = key.substr(not_space);
            val = str.substr(delim + 1);
            if(!key.compare("SID"))
                break;
        }
    }
    else
    {
        size_t delim = value.find('=');
        key = value.substr(0, delim);
        val = value.substr(delim + 1);
    }
    std::string sessions_path = "sessions";
    DIR *Dir = opendir(sessions_path.c_str());
    if (Dir == NULL)
    {
        status = 500;
        return;
    }
    struct dirent *entry = readdir(Dir);
    std::string EntryString;
    while(entry)
    {
        EntryString = entry->d_name;
        if (!EntryString.compare(val))
        {
            if (!access((sessions_path + "/" + val).c_str(), F_OK | R_OK))
                envMap["SID"]=val;
            else
                status = 500;
            closedir(Dir);
            return;
        }
        entry = readdir(Dir);
    }
    closedir(Dir);
}
void CgiEnv::setHttpCookies(std::string &value)
{
    if(!value.empty())
    {
        envMap["HTTP_COOKIE"] = value;
        processAndSetSessions(value);
    }
        
}
// void CgiEnv::setRequestMethod()
// {
//     envMap["REQUEST_METHOD"]="GET";
// }
void CgiEnv::setBoundry(const std::string &bodyBoundary)
{
    boundary = bodyBoundary;
}
void CgiEnv::setPathUriVector()
{
    std::string path = worker.getPath();
    std::string value;

    if (!path.compare("/cgi-bin"))
        path.push_back('/');
    std::cout<<path<<std::endl;
    // exit(1);
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
    for (size_t i = 0; i < pathUri.size(); i++)
    {
        std::cout<<pathUri[i]<<std::endl;
    }
    // exit(1);
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
            {    cgiDir = true;
                envMap["ROOT"] = cgiRoot;}
            else if (isValidDir == -1 || accessDir == -1)
            {
                if (accessDir == -1)
                    status = 403;
                else
                    status = 404;
            }
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
void CgiEnv::setInputFromBody()
{
    std::cout<<RED<<reqBody<<RESET<<std::endl;
    exit(1);
    std::string body;
    if (boundary.size())
    {
        std::string delimiter = boundary.substr(0, boundary.size() - 2);
        std::stringstream stream(reqBody);
        std::string line;
        bool value = false;
        while(getline(stream, line))
        {
            if (!line.compare(delimiter))
                continue;
            else if (line.size() && line[0] == '\r' && value == false)
            {
                value = true;
                continue;
            }
            else if (value)
            {
                body.push_back('=');
                for (size_t i = 0; i < line.size(); i++)
                {
                    if (line[i] != '\n' && line[i] != '\r')
                        body.push_back(line[i]);
                }
                body.push_back('&');
                value = false;
            }
            else if (line.find("Content-Disposition") != std::string::npos)
            {
                size_t index = line.find("name=");
                if (index == std::string::npos)
                {
                    index = line.find("filename=");
                }
                if (index != std::string::npos)
                {
                    std::string tmp = line.substr(index);
                    size_t i = tmp.find("\"");
                    if (i != std::string::npos)
                    {
                        while (tmp[++i] != '\"')
                        {
                            body.push_back(tmp[i]);
                            // i++;
                        }
                    }
                    
                }
            }
        }
        body.pop_back();
        // std::cout<<YELLOW<<body<<RESET<<std::endl;
        // exit(1);
        reqBody = body;
    }
}
void CgiEnv::setCgiPATHINFO()
{
    std::string root;
    std::string path;
    std::string currentDir = cgiRoot;
    std::string extraPath;
    // extraPath += cgiRoot;
    if (extraPathIndex != -1)
    {
        if (validRoot && cgiDir && cgiScript)
        {
            for (size_t i = extraPathIndex; i < pathUri.size(); i++)
            {
                extraPath += "/"; 
                extraPath += pathUri[i];
            }
            // if (!access(extraPath.c_str(), F_OK | R_OK | X_OK))
            // {
            //     extraPath = true;
            // }
            // else
            //     errno == EACCES ? status = 403 : status = 404;
                envMap["PATH_INFO"] = extraPath;
        }
    }
}
bool CgiEnv::isValidscript(std::string &script)
{
    size_t ext = script.find_last_of(".");
    bool isvalid = true;
    if (ext != std::string::npos)
    {
        std::string extForm = script.substr(ext);
        if (!extForm.compare(".py"))
            scriptBin = worker.get_python_bin();
        else if (!extForm.compare(".sh"))
            scriptBin = worker.get_bash_bin();
        else
            isvalid = false;
    }
    else
        isvalid = false;
    return isvalid;
}
void CgiEnv::findScript()
{
    DIR *Dir;
    std::string currentDir = cgiRoot;
    int is_file = 0;
    bool next = false;
    bool isHandledScript = false;
    
    if (validRoot && cgiDir)
    {
        for (size_t i = 0; i < pathUri.size() - 1; i++)
        {
            is_file = Is_Directory(currentDir);

            if(!is_file)
            {
                Dir = opendir(currentDir.c_str());
                if (Dir == NULL)
                {
                    errno == EACCES ? status = 403 : status = 404;
                    return;
                }
                struct dirent *entry = readdir(Dir);
                std::string entryString;
                while (entry)
                {
                    entryString = entry->d_name;
                    if (!entryString.compare(pathUri[i + 1]))
                    {
                        next =  true;
                        is_file =  Is_Directory(currentDir  + "/" + pathUri[i + 1]);
                        if (is_file)
                        {
                            isHandledScript = isValidscript(pathUri[i + 1]);
                            int access_ret = access((currentDir  + "/" + pathUri[i + 1]).c_str(), F_OK | R_OK | X_OK);
                            if (isHandledScript && !access_ret)
                            {
                                cgiScript = true;
                                envMap["SCRIPT_NAME"] = currentDir  + "/" + pathUri[i + 1];
                                if (i + 2 < pathUri.size())
                                    extraPathIndex = i + 2;
                            }
                            else if(!isHandledScript)
                                status = 501;
                            else
                                errno == EACCES ? status = 403 : status = 404;
                            closedir(Dir);
                            return;
                        }
                        else if (!is_file)
                            currentDir = currentDir  + "/" + pathUri[i + 1];
                        else
                           { status = 404;
                            return;}
                        break;
                    }
                    entry = readdir(Dir);
                }
                closedir(Dir);
                if (!next)
                {   
                    status = 404;
                    return ;
                }
            }
            else if (is_file == 1)
            {
                isHandledScript = isValidscript(pathUri[i + 1]);
                int access_ret = access((currentDir  + "/" + pathUri[i + 1]).c_str(), F_OK | R_OK | X_OK);
                if (isHandledScript && !access_ret)
                {
                    cgiScript = true;
                    envMap["SCRIPT_NAME"] = currentDir  + "/" + pathUri[i + 1];
                    if (i + 2 < pathUri.size())
                        extraPathIndex = i + 2;
                }
                else if(!isHandledScript)
                    status = 501;
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
        Dir = opendir(currentDir.c_str());
        if (Dir == NULL)
        {
            errno == EACCES ? status = 403 : status = 404;
            return;
        }
        if (worker.getIndex().size())
        {
            struct dirent *entry = readdir(Dir);
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
                    closedir(Dir);
                    return;
                }
                entry = readdir(Dir);
            }
            closedir(Dir);
        }
        if (!cgiScript && worker.getAutoIndex().size() && !worker.getAutoIndex().compare("on"))
        {
            autoIndex = true;
            status = 403;
        }
        else
            status = 404;
    }

}

// void CgiEnv::constructScriptEnv()
// {
//     size_t env_size = envMap.size();
//     std::map<std::string, std::string>::iterator it;
//     std::string data;
//     int count = 0;
//     if (env_size)
//     {
//         cgiMetaData = new char* [env_size + 1];
//         for (it = envMap.begin(); it != envMap.end(); it++)
//         {
//             data = it->first + "=" + it->second;
//             cgiMetaData[count] = strdup(data.c_str());
//             count++;
//         }
//         cgiMetaData[count] = NULL;
//     }
//     for (size_t i = 0; cgiMetaData[i] != NULL; i++)
//     {
//         printf("%s\n", cgiMetaData[i]);
//     }
    
// }
void CgiEnv::setStatusCode(int code)
{
    status = code;
}
bool CgiEnv::isAllowedMethod()
{
    std::vector<std::string> allowed = worker.getAllowMethods();
    for (size_t i = 0; i < allowed.size(); i++)
    {
        if (!allowed[i].compare(envMap["REQUEST_METHOD"]))
            return true;
    }
    return false;
    
}
void CgiEnv::setErrorpage()
{
    if (!cgiDir )
        status = 404;
    if (!isAllowedMethod() && status != 501)
        status = 405;
    unsigned int error_status = 0;
    if (autoIndex)
        error_status = 403;
    else if (status != 0)
        error_status = status;
    else
        errorPage = "valid request";
    if (error_status != 0)
    {
        worker.setPathError(worker.getErrorPages(), error_status);
        if (worker.get_track_status() && worker.getPathError().size())
           errorPage = worker.getPathError();
    }
}
void CgiEnv::setRedirection()
{
    std::string redir = worker.getRedirect();
    if (redir.size())
    {
        redirection = redir;
        isRedir = true;
    }
}
void CgiEnv::setEnvironementData()
{
        setPathUriVector();
        setCgiRoot();
        findScript();
        setCgiPATHINFO();
        setCgiQueryString();
        setCgiServerName();
        setCgiServePort();
        setErrorpage();
        envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
        setRedirection();


        // exit(1);
        

    // std::cerr <<envMap["CONTENT_TYPE"]<<std::endl;
    // std::cerr <<envMap["CONTENT_LENGTH"]<<std::endl;
    // std::cerr<<reqBody<<std::endl;
}
void CgiEnv::setRequest(const std::string &req)
{
    envMap["REQUEST_METHOD"] = req;
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
std::string &CgiEnv::getInputFromBody()
{
    return reqBody;
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
int CgiEnv::getStatus()
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
// char **CgiEnv::getenvArray()
// {
//     return cgiMetaData;
// }
bool CgiEnv::isAutoIndexReq()
{
    return autoIndex;
}
const std::map<std::string, std::string> &CgiEnv::getEnvMap() const
{
    return (envMap);
}
std::string &CgiEnv::getErrorPage()
{
    return errorPage;
}
std::string &CgiEnv::getBoundary()
{
    return boundary;
}
std::string &CgiEnv::getScriptBin()
{
    return scriptBin;
}
bool CgiEnv::getRedirectionStatus()
{
    return isRedir;
}
std::string &CgiEnv::getRedirectionpage()
{
    return redirection;
}
Worker &CgiEnv::getWorker()
{
    return worker;
}