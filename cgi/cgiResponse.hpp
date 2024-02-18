/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 14:12:57 by aalami            #+#    #+#             */
/*   Updated: 2024/02/18 20:55:23 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_RESPONSE
#define CGI_RESPONSE
#include "cgi.hpp"
#include "../http.server.hpp"
#include <sstream>

class CgiResponse
{
private:
    CgiEnv Env;
    int socket_fd;
    char **scriptData;
    bool responseSent;
    bool isDataset;
    bool isEnvObjectSet;
    bool isErrorResponse;
    std::map<unsigned int, std::string> status_codes;
    int errorpipe[2];
    int trackerPipe[2];
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
    bool isResponseSent();
    bool isEnvset();
    bool isReqObjectset();
    bool isError();
};


#endif