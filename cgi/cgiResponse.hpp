/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 14:12:57 by aalami            #+#    #+#             */
/*   Updated: 2024/03/09 02:31:40 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_RESPONSE
#define CGI_RESPONSE
#include "cgi.hpp"
#include "../http.server.hpp"
#include <sstream>
#include<ctime>
#define RESP_TIMEOUT 10

class CgiResponse
{
private:
    CgiEnv Env;
    std::string responseStr;
    int socket_fd;
    int tmp_socket;
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
    int trackerPipe[2];
    int inputPipe[2];
    std::string errorResponse;
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
};


#endif