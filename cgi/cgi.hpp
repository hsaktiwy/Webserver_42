/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 12:14:06 by aalami            #+#    #+#             */
/*   Updated: 2024/02/13 16:15:54 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP
#include "../http.server.hpp"
#include "../request.hpp"


//script-URI = <scheme> "://" <server-name> ":" <server-port><script-path> <extra-path> "?" <query-string>
class CgiEnv
{
private:
    std::map<std::string, std::string> envMap;
    std::vector<std::string> pathUri;
    Worker worker;
    char **cgiMetaData;
    std::string cgiRoot; //full path to the Root dir of cgi "cgi-bin"
    bool validRoot; //check if the root of the request is present and could be opened
    bool cgiDir; //is the Dir of cgi ("cgi-bin") is present
    bool cgiScript; //is the cgi script found
    bool extrapath; //is the cgi script found
    bool autoIndex;
    bool isDir;
    bool isFile;
    int status;
    int extraPathIndex;
    
public:
    CgiEnv(const Worker &workerObj);
    void setPathUriVector();
    void setCgiRoot();
    void setCgiServerName();
    void setCgiServePort();
    // void setCgiScriptName();
    void setCgiPATHINFO();
    void setCgiQueryString();
    void findScript();
    void setRequestMethod();
    void constructScriptEnv();
    void setEnvironementData();
    bool isDirectoryRequest();
    std::string &getCgiServerName();
    std::string &getCgiServerPort();
    std::string &getCgiScriptName();
    std::string &getCgiPATHINFO();
    std::string &getCgiQueryString();
    std::string &getCgiRoot();
    bool getStatus();
    bool getCgiDirStatus();
    bool isScriptFound();
    char **getenvArray();
};

#endif