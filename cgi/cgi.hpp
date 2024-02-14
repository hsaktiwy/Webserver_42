/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaktiwy <hsaktiwy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 12:14:06 by aalami            #+#    #+#             */
/*   Updated: 2024/02/14 17:39:48 by hsaktiwy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP
#include "../http.server.hpp"

//script-URI = <scheme> "://" <server-name> ":" <server-port><script-path> <extra-path> "?" <query-string>
class CgiEnv
{
private:
    std::map<std::string, std::string> envMap;
    Worker worker;
    char **cgiMetaData;
public:
    CgiEnv(const Worker &workerObj);
    void setCgiServerName();
    void setCgiServePort();
    void setCgiScriptPath();
    // void setCgiPATHINFO();
    void setCgiQueryString();
    std::string &getCgiServerName();
    std::string &getCgiServerPort();
    // std::string getCgiScriptPath();
    // std::string getCgiPATHINFO();
    std::string &getCgiQueryString();
};

#endif