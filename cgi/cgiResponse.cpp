/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 16:13:26 by aalami            #+#    #+#             */
/*   Updated: 2024/03/16 23:02:30 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgiResponse.hpp"
#include <fstream>

CgiResponse::CgiResponse() : scriptData(NULL)
{
    responseSent = false;
    isDataset = false;
    isEnvObjectSet = false;
    isErrorResponse = false;
    processSpawned = false;
    responseOnProcess = false;
    processSpawned = 0;
    socket_fd = -1;
    tmp_fd= -1;
    processId= -1;
}
CgiResponse::~CgiResponse()
{
    if (scriptData != NULL)
    {
        size_t i = 0;
        while (scriptData[i] != NULL)
        {
            delete [] scriptData[i];
            i++;
        }
        delete [] scriptData[i];
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
        errorResponse = obj.errorResponse;
        isDataset = obj.isDataset;
        isEnvObjectSet = obj.isEnvObjectSet;
        status_codes = obj.status_codes;
        processSpawned = obj.processSpawned;
        processTime = obj.processTime;
        tmp_fd = obj.tmp_fd;
        processId = obj.processId;
        responseStr = obj.responseStr;
        responseOnProcess = obj.responseOnProcess;
        errorpipe[0] = obj.errorpipe[0];
        errorpipe[0] = obj.errorpipe[0];
        trackerPipe[0] = obj.trackerPipe[0];
        trackerPipe[1] = obj.trackerPipe[1];
        inputPipe[0] = obj.inputPipe[0];
        inputPipe[1] = obj.inputPipe[1];
        if (scriptData != NULL)
        {
            size_t size = Env.getEnvMap().size();
            for (size_t i = 0; i < size; i++)
                delete [] scriptData[i];
            delete [] scriptData[size];
            delete [] scriptData;
            scriptData = NULL;
        }
        // constructScriptEnv();
    }
    return (*this);
}
int CgiResponse::getsocket()
{
    return socket_fd;
}
void CgiResponse::setErrorMap(std::map<unsigned int, std::string> &ss_c)
{
    status_codes = ss_c;
}
void CgiResponse::setSocket(int fd)
{
    socket_fd = fd;
}
void CgiResponse::creatCgiResponse()
{
    if (!Env.getStatus() && !isErrorResponse && !Env.getRedirectionStatus())
    {
        if(!processSpawned)
        {
            // printf("%s\n", Env.getCgiScriptName().c_str());
            // exit(1);
            int errorPipeReturn = pipe(errorpipe);
            fcntl(errorpipe[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
            fcntl(errorpipe[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
            std::string path_bin = Env.getScriptBin();
            char **args;
            // int status;
            // tmp_socket = dup(socket_fd);
            args = new char *[3];
            args[0] = (char *)path_bin.c_str();
            args[1] = (char *)Env.getCgiScriptName().c_str();

            args[2] = NULL;
            // processSpawned = true;
            int pid = fork();
            if (errorPipeReturn == -1 || pid == -1)
            {
                Env.setStatusCode(500);
                Env.setErrorpage();
                isErrorResponse = true;
                handleError();
                return;
            }
            else
            {
                if (pid == 0)
                {
                    int outfd = open("/tmp/outfile", O_CREAT | O_RDWR,0644);
                    bool flag = false;
                    int fd;
                    std::map<std::string, std::string> tmp = Env.getEnvMap();
                    if (!tmp["REQUEST_METHOD"].compare("POST"))
                    {
                        std::string str = Env.getInputFromBody();
                        const char *data = str.data();
                        fd = open("/tmp/tmpFile", O_CREAT | O_RDWR,0644);
                        if (fd == -1 || outfd == -1)
                            perror("open :");
                        int bytes  = write(fd, data, str.size());
                        if (bytes == -1)
                            perror("write :");
                        close(fd);  
                        flag = true;
                    }
                    close (STDOUT_FILENO);
                    close (STDERR_FILENO);
                    close (errorpipe[0]);
                    // close (trackerPipe[0]);
                    dup2 (errorpipe[1], 2);
                    dup2(outfd, 1);
                    close(outfd);
                    if (flag)
                    {
                        close(STDIN_FILENO);
                        open("/tmp/tmpFile", O_RDWR);
                        dup2(fd, 0);
                        close (fd);
                    }
                    if (execve(Env.getCgiScriptName().c_str(), args, scriptData) == -1)
                        std::cerr<<errno;
                        // perror("execve");
                    close(errorpipe[1]);
                }
                
                else
                {
                    processTime = clock();
                    processId = pid;
                    if (!processSpawned)
                    {
                        
                        // close(inputPipe[0]);
                        // close(inputPipe[1]);
                        close(errorpipe[1]);
                        processSpawned = true;
                        delete [] args;
                        // dup2(socket_fd, tmp_socket);
                    }
                }
            }
        }
        else
        {
            if (!responseOnProcess)
            {
                char buffer[CHUNK_SIZE];
                // char processBuffer[CHUNK_SIZE];
                int errorchecker = read(errorpipe[0], buffer, CHUNK_SIZE);
                // int processchecker = read(trackerPipe[0], processBuffer, CHUNK_SIZE);
                // printf("waaaaaa %d %s\n", errorchecker ,processBuffer);
                if (errorchecker != -1 )
                {
                    if (errorchecker == 0)
                    {
                        responseOnProcess = true;
                        // responseSent = true;
                    }
                    else
                    {
                        Env.setStatusCode(500);
                        Env.setErrorpage();
                        isErrorResponse = true;
                        handleError();
                        close(errorpipe[0]);
                        close(trackerPipe[0]);
                        return;
                    }
                    close(errorpipe[0]);
                }
                else
                {
                    clock_t current = clock();
                    double timeSpent = static_cast<double>(current - processTime) / CLOCKS_PER_SEC;
                    // printf("current = %ld processTime = %ld fd = %d\n ", current / CLOCKS_PER_SEC, processTime / CLOCKS_PER_SEC, socket_fd);
                    if (timeSpent >= RESP_TIMEOUT)
                    {
                        kill(processId, SIGINT);
                        Env.setStatusCode(504);
                        Env.setErrorpage();
                        isErrorResponse = true;
                        handleError();
                        close(errorpipe[0]);
                        close(trackerPipe[0]);
                        return;
                    }
                    
                }
            }
            else
                processResponse();
        }
    }
    else
    {
        if (!Env.getRedirectionStatus())
            handleError();
        else
            handleRedirection();
    }
}
void CgiResponse::handleRedirection()
{
    std::string redirPath = Env.getRedirectionpage();
    std::string response;
    response += "HTTP/1.1 302 Found\r\n";
    response += "Location: ";
    response += redirPath + "\r\n\r\n";
    send(socket_fd, response.c_str(), response.size(), 0);
    responseSent=true;
}
bool CgiResponse::isPostMethod()
{
    std::map<std::string, std::string> tmp = Env.getEnvMap();
    if (!tmp["REQUEST_METHOD"].compare("POST"))
            return true;
    return false;
}
void CgiResponse::processResponse()
{
    static int i;
    char buff_resp[CHUNK_SIZE + 1];
    if (tmp_fd == -1)
    {
        tmp_fd = open("/tmp/outfile", O_RDONLY);
        fcntl(tmp_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
        if (tmp_fd == -1)
        {
            Env.setStatusCode(500);
            Env.setErrorpage();
            isErrorResponse = true;
            handleError();
            return;
        }
    }
    int bytesRead  = read(tmp_fd, buff_resp, CHUNK_SIZE);
    i += bytesRead;
    if (bytesRead != -1)
    {
        if (bytesRead == 0)
       {
            responseSent = true;
            if (Env.getStatus() == 0)
                Env.setStatusCode(200);
            close(tmp_fd);
            std::remove("/tmp/outfile");
            if (isPostMethod())
                std::remove("/tmp/tmpFile");
       }
        else
        {
            buff_resp[bytesRead] = 0;
            send(socket_fd, buff_resp, bytesRead, 0);
            for (int i = 0; i < bytesRead; i++)
                responseStr += buff_resp[i];
        }
        
    }
}
void CgiResponse::setCgiEnvObject(CgiEnv &obj)
{
    
    Env = obj;
    isEnvObjectSet = true;
    setErrorResponseState();
    // constructScriptEnv();
}
void CgiResponse::setErrorResponseState()
{
    if ((Env.getErrorPage().size() && Env.getErrorPage().compare("valid request")) || Env.getErrorPage().empty())
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
    
    
}
void CgiResponse::handleError()
{
    if (Env.isAutoIndexReq() || Env.getStatus())
    {
        if (Env.getErrorPage().size() && Env.getErrorPage().compare("valid request"))
        {
            errorResponse += "HTTP/1.1 302 Found\r\n";
            errorResponse += "Location: ";
            errorResponse += Env.getErrorPage() + "\r\n";
            errorResponse += "Content-Type: text/html\r\n\r\n";
        }
        else
        {
            std::stringstream stream;
            std::string body;
            stream << Env.getStatus() << " "<< status_codes[Env.getStatus()];
            errorResponse += "HTTP/1.1 "+ stream.str() + "\r\n";
            errorResponse += "Content-Type: text/html\r\n";
            body += "<!DOCTYPE html>\r\n";
            body += "<html lang=\"en\">\r\n";
            body += "<head>\r\n";
            body += "    <meta charset=\"UTF-8\">\r\n";
            body += "    <title>" + stream.str() + "</title>\r\n";
            body += "</head>\r\n";
            body += "<body>\r\n";
            body += "    <h1>" + stream.str() + "</h1>\r\n";
            body += "</body>\r\n";
            body += "</html>";
            stream.str("");
            stream << body.size();
            errorResponse += "Content-Length: " + stream.str() +"\r\n\r\n";
            errorResponse += body;
        }
        // if (processSpawned)
        //     {send(tmp_socket, errorResponse.c_str(), errorResponse.size(), 0); close(tmp_socket);}
        // else
            send(socket_fd, errorResponse.c_str(), errorResponse.size(), 0);
        responseSent=true;
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
bool CgiResponse::isError()
{
    return isErrorResponse;
}
bool CgiResponse::isProcessSpawned()
{
    return processSpawned;
}
