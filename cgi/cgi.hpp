/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 12:14:06 by aalami            #+#    #+#             */
/*   Updated: 2024/02/29 20:40:09 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP
#include "../http.server.hpp"
#include "../request.hpp"
#include <sstream>


//script-URI = <scheme> "://" <server-name> ":" <server-port><script-path> <extra-path> "?" <query-string>
class CgiEnv
{
private:
    std::map<std::string, std::string> envMap;
    std::vector<std::string> pathUri;
    Worker worker;
    std::string errorPage;
    std::string cgiRoot; //full path to the Root dir of cgi "cgi-bin"
    std::string reqBody;
    std::string boundary;
    std::string contentType;
    std::string contentLength;
    bool validRoot; //check if the root of the request is present and could be opened
    bool cgiDir; //is the Dir of cgi ("cgi-bin") is present
    bool cgiScript; //is the cgi script found
    bool extrapath; //is the cgi script found
    bool autoIndex;
    bool isDir;
    bool isFile;
    bool ismetaDataset;
    bool isErrorpageSet;
    int status;
    int extraPathIndex;
    
public:
    CgiEnv();
    CgiEnv(const Worker &workerObj);
    CgiEnv(const CgiEnv &obj);
    CgiEnv &operator=(const CgiEnv &obj);
    void setCgiWorker(const Worker &obj);
    void setContentType(std::string &value);
    void setContentLength(std::string &value);
    void setPathUriVector();
    void setCgiRoot();
    void setCgiServerName();
    void setCgiServePort();
    void setRequest(const std::string &req);
    // void setCgiScriptName();
    void setCgiPATHINFO();
    void setCgiQueryString();
    void setRequestBody(const std::string &body);
    void setBoundry(const std::string &bodyBoundary);
    void findScript();
    void setErrorpage();
    void setStatusCode(int code);
    void setInputFromBody();
    void setHttpCookies(std::string &value);
    // void setRequestMethod();
    // void constructScriptEnv();
    void setEnvironementData();
    bool isDirectoryRequest();
    std::string &getCgiServerName();
    std::string &getCgiServerPort();
    std::string &getCgiScriptName();
    std::string &getCgiPATHINFO();
    std::string &getCgiQueryString();
    std::string &getCgiRoot();
    std::string &getInputFromBody();
    std::string &getBoundary();
    int getStatus();
    bool isAutoIndexReq();
    bool getCgiDirStatus();
    bool isScriptFound();
    std::string &getErrorPage();
    // char **getenvArray();
    const std::map<std::string, std::string> &getEnvMap() const;
};

#endif