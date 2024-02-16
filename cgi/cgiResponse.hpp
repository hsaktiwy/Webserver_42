/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 14:12:57 by aalami            #+#    #+#             */
/*   Updated: 2024/02/15 22:23:40 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_RESPONSE
#define CGI_RESPONSE
#include "cgi.hpp"
#include "../http.server.hpp"

class CgiResponse
{
private:
    CgiEnv Env;
    int socket_fd;
    char **scriptData;
    bool responseSent;
    bool isDataset;
    bool isEnvObjectSet;
    int errorpipe[2];
    int trackerPipe[2];
public:
    CgiResponse();
    ~CgiResponse();
    CgiResponse(const CgiResponse &obj);
    CgiResponse &operator=(const CgiResponse &obj);
    void constructScriptEnv();
    void setCgiEnvObject(CgiEnv &obj);
    void setSocket(int fd);
    void creatCgiResponse();
    void handleError();
    bool isResponseSent();
    bool isEnvset();
    bool isReqObjectset();
};


#endif