/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 16:13:26 by aalami            #+#    #+#             */
/*   Updated: 2024/03/25 23:22:24 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cgiResponse.hpp"
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
    errorpipe[0]= -1;
    errorpipe[1] = -1;
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
        outfile = obj.outfile;
        infile = obj.infile;
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
            outfile = "/tmp/out" + convertToString(socket_fd);
            infile = "/tmp/in" + convertToString(socket_fd);
            int errorPipeReturn = pipe(errorpipe);
            int error_re = fcntl(errorpipe[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
            int error_we = fcntl(errorpipe[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
            std::string path_bin = Env.getScriptBin();
            char **args;
            args = new char *[3];
            args[0] = (char *)path_bin.c_str();
            args[1] = (char *)Env.getCgiScriptName().c_str();
            args[2] = NULL;
            int pid;
            if (errorPipeReturn == -1 ||  error_re == -1 || error_we == -1)
            {
                if (!access(outfile.c_str(), F_OK))
                {
                    close(tmp_fd);
                    std::remove(outfile.c_str());
                }
                delete [] args;
                close(errorpipe[0]);
                close(errorpipe[1]);
                Env.setStatusCode(500);
                Env.setErrorpage();
                isErrorResponse = true;
                handleError();
                return;
            }
            else
            {
                pid = fork();
                if (pid == -1)
                {
                    if (!access(outfile.c_str(), F_OK))
                    {
                        close(tmp_fd);
                        std::remove(outfile.c_str());
                    }
                    delete [] args;
                    close(errorpipe[0]);
                    close(errorpipe[1]);
                    Env.setStatusCode(500);
                    Env.setErrorpage();
                    isErrorResponse = true;
                    handleError();
                    return;
                    
                }
                if (pid == 0)
                {
                    
                    if (close (errorpipe[0]) == -1 || dup2 (errorpipe[1], 2) == -1)
                    {
                        perror("error ");
                        close(errorpipe[1]);
                        exit(EXIT_FAILURE);
                    }   
                    close(errorpipe[1]);
                    int outfd = open(outfile.c_str(), O_CREAT | O_RDWR,0644);
                    bool flag = false;
                    int fd;
                    std::map<std::string, std::string> tmp = Env.getEnvMap();
                    if (!tmp["REQUEST_METHOD"].compare("POST"))
                    {
                        std::string str = Env.getInputFromBody();
                        const char *data = str.data();
                        fd = open(infile.c_str(), O_CREAT | O_RDWR,0644);
                        int bytes  = write(fd, data, str.size());
                        if (bytes == -1 || fd == -1 || outfd == -1)
                        {
                            perror("error :");
                            if (!access(outfile.c_str(), F_OK))
                            {
                                close(outfd);
                                std::remove(outfile.c_str());
                            }
                            if (!access(infile.c_str(), F_OK))
                            {
                                close(fd);
                                std::remove(infile.c_str());
                            }
                            exit(EXIT_FAILURE);
                        }
                        close(fd);
                        flag = true;
                    }
                    if (flag)
                    {
                        fd = open(infile.c_str(), O_RDWR, 0644);
                        if (dup2(fd, 0) == -1)
                        {
                            perror("dup2 ");
                            exit(EXIT_FAILURE);
                        }
                        close (fd);
                    }
                    if (dup2(outfd, 1) == -1)
                    {
                        perror("dup2 ");
                        exit(EXIT_FAILURE);
                    }
                    close(outfd);
                    if (execve(path_bin.c_str(), args, scriptData) == -1)
                    {
                        perror("execve : ");
                        close(errorpipe[1]);
                        exit(EXIT_FAILURE);
                    }
                }
                
                else
                {
                    processTime = clock();
                    processId = pid;
                    if (!processSpawned)
                    {
                        close(errorpipe[1]);
                        processSpawned = true;
                        delete [] args;
                    }
                }
            }
        }
        else
        {
            if (!responseOnProcess)
            {
                char buffer[CHUNK_SIZE + 1];
                int errorchecker = read(errorpipe[0], buffer, CHUNK_SIZE);
                if (errorchecker != -1 )
                {
                    if (errorchecker == 0)
                    {
                        responseOnProcess = true;
                        close(errorpipe[0]);
                    }
                    else
                    {
                        kill(processId, SIGKILL);
                        if (!access(outfile.c_str(), F_OK))
                            std::remove(outfile.c_str());
                        if (!access(infile.c_str(), F_OK))
                            std::remove(infile.c_str());
                        Env.setStatusCode(500);
                        Env.setErrorpage();
                        isErrorResponse = true;
                        handleError();
                        close(errorpipe[0]);
                        return;
                    }
                }
                else
                {
                    clock_t current = clock();
                    double timeSpent = static_cast<double>(current - processTime) / CLOCKS_PER_SEC;
                    if (timeSpent >= RESP_TIMEOUT)
                    {
                        kill(processId, SIGKILL);
                        Env.setStatusCode(504);
                        Env.setErrorpage();
                        isErrorResponse = true;
                        if (!access(outfile.c_str(), F_OK))
                            std::remove(outfile.c_str());
                        if (!access(infile.c_str(), F_OK))
                            std::remove(infile.c_str());
                        handleError();
                        close(errorpipe[0]);
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
        tmp_fd = open(outfile.c_str(), O_RDONLY);
        int fc = fcntl(tmp_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
        if (tmp_fd == -1 || fc == -1)
        {
            if (!access(outfile.c_str(), F_OK))
            {
                close(tmp_fd);
                std::remove(outfile.c_str());
            }
            kill(processId, SIGKILL);
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
            std::remove(outfile.c_str());
            if (isPostMethod())
                std::remove(infile.c_str());
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
            stream << Env.getStatus();

            std::map<std::string, std::string> a = Env.getEnvMap();
            errorResponse += "HTTP/1.1 "+ stream.str() + "\r\n";
            errorResponse += "Content-Type: text/html\r\n";
            body += "<!DOCTYPE html>\r\n";
            body += "<html lang=\"en\">\r\n";
            body += "<head>\r\n";
            body += "    <meta charset=\"UTF-8\">\r\n";
            body += "    <title>Error Page</title>\r\n";
            body += "<style>\r\nbody {\r\n";
            body += "font-family: Arial, sans-serif;\r\ntext-align: center;\r\n";
            body += "padding-top: 50px;\r\n}\r\nh1 {\r\nfont-size: 3em;\r\ncolor: #990000;\r\n";
            body += "margin-bottom: 20px;\r\n}\r\np {\r\nfont-size: 1.5em;\r\ncolor: #666666;\r\n";
            body += "margin-bottom: 50px;\r\n}\r\n</style>\r\n";
            body += "</head>\r\n";
            body += "<body>\r\n";
            body += "    <h1> Error " + stream.str() + "(" + status_codes[Env.getStatus()]+ ")"+"</h1>\r\n";
            body += "<p>Unable to reserve a propore response.</p>\r\n";
            body += "</body>\r\n";
            body += "</html>";
            stream.str("");
            stream << body.size();
            errorResponse += "Content-Length: " + stream.str() +"\r\n\r\n";
            errorResponse += body;
        }
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

int CgiResponse::getoutfilesocket()
{
    return tmp_fd;
}
std::string &CgiResponse::getoutfilename()
{
    return outfile;
}
int CgiResponse::getpipeReadEnd()
{
    return errorpipe[0];
}
int CgiResponse::getpipeWriteEnd()
{
    return errorpipe[1];
}
int CgiResponse::getprocessId()
{
    return processId;
}