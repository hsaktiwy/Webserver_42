/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 14:12:57 by aalami            #+#    #+#             */
/*   Updated: 2024/02/14 22:01:46 by aalami           ###   ########.fr       */
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
    char **scriptData;
    int socket_fd;
    bool responseSent;
    int errorpipe[2];
    int trackerPipe[2];
public:
    CgiResponse();
    ~CgiResponse();
    CgiResponse(const CgiResponse &obj);
    CgiResponse &operator=(const CgiResponse &obj);
    void constructScriptEnv();
    void setCgiEnvObject(CgiEnv &obj);
    // void creatCgiResponse();
    void handleError();
    bool isResponseSent();
};


#endif