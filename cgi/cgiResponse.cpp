/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 16:13:26 by aalami            #+#    #+#             */
/*   Updated: 2024/02/10 17:06:22 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgiResponse.hpp"

cgiResponse::cgiResponse(CgiEnv &obj) : Env(obj)
{
    
}
void cgiResponse::creatCgiResponse()
{
    if (!Env.getStatus())
    {
        int pid = fork();
        std::string path_bin = "/usr/local/bin/python3";
        char **args;
        args = new char *[3];
        args[0] = (char *)path_bin.c_str();
        args[1] = (char *)Env.getCgiScriptName().c_str();
        args[2] = NULL;
        if (fork == 0)
        {
            execve(Env.getCgiScriptName().c_str(), args, Env.getenvArray());
        }
    }
}