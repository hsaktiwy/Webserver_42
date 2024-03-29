/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 14:12:57 by aalami            #+#    #+#             */
/*   Updated: 2024/03/25 22:49:27 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_RESPONSE
#define CGI_RESPONSE
#include "cgi.hpp"
#include "http.server.hpp"
#include <sstream>
#include<ctime>
#define RESP_TIMEOUT 10

class CgiResponse
{
private:
    CgiEnv Env;
    std::string responseStr;
    int socket_fd;
    int tmp_fd;
    int processId;
    clock_t processTime;
    char **scriptData;
    bool responseSent;
    bool isDataset;
    bool isEnvObjectSet;
    bool isErrorResponse;
    bool processSpawned;
    bool responseOnProcess;
    std::map<unsigned int, std::string> status_codes;
    int errorpipe[2];
    std::string errorResponse;
    std::string outfile;
    std::string infile;
public:
    CgiResponse();
    ~CgiResponse();
    CgiResponse(const CgiResponse &obj);
    CgiResponse &operator=(const CgiResponse &obj);
    void constructScriptEnv();
    void setErrorResponseState();
    void setCgiEnvObject(CgiEnv &obj);
    void setSocket(int fd);
    void setErrorMap(std::map<unsigned int, std::string> &ss_c);
    void creatCgiResponse();
    void handleError();
    void handleRedirection();
    bool isResponseSent();
    bool isEnvset();
    bool isReqObjectset();
    bool isError();
    bool isProcessSpawned();
    void processResponse();
    int getsocket();
    int getprocessId();
    int getoutfilesocket();
    std::string &getoutfilename();
    int getpipeReadEnd();
    int getpipeWriteEnd();
    bool isPostMethod();
};


#endif