/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   responseHandler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 18:12:26 by aalami            #+#    #+#             */
/*   Updated: 2024/01/24 17:27:07 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
// bool isQuerySet(std::string &str)
// {
//     std::stringstream stream(str);
    
//     std::string query;
//     if (getline(stream, query, ' '))//get the ressource with the query if any
//     {
//         std::cout<<query<<std::endl;
//     }
//     if(str.find("?") == std::string::npos)
//         return 0;
//     return 1;
// }

// // std::string getQuery(std::string &str)
// // {
// //     std::string query;
    
// // }
// char **generateCgiEnv(std::map<std::string, std::string> &request)
// {
//     // int envCount;
//     char **env;
//     std::string var; 

//     env = (char **)malloc(sizeof(char *) * 2);
    
//     mapstr_it it = request.find("GET");
//     if (it != request.end())
//     {
//         if(isQuerySet(it->second))
//         {
//             var = "QUERY_STRING=";
//             // var+= getQuery(it->second);
//             // env[0] = strdup
//         }
//     }
//     return NULL;
// }
void responseHandler(std::vector<struct pollfd> &fds, int index, char **env,
    std::map<std::string,std::string> &request)
{
    // std::ifstream stream;
    // stream.open("index.html");
    // std::string buffer;
    // getline(stream, buffer, '\0');
    (void)request;
    (void)env;
    std::string httpResponse =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 5\r\n"
        "Hello"
        "\r\n";
    
    const char *buff = httpResponse.c_str();
    send(fds[index].fd, buff, strlen(buff),0 );
    // char *path = strdup("/usr/bin/python3");
    // std::string cgi = "/Users/aalami/Desktop/Webserver_42/cgi-bin/cgi.py";
    // char **arg ;
    // // int status;
    // arg = (char **)malloc(sizeof(char *) * 3);
    // arg[0] = path;
    // arg[1] = strdup(cgi.c_str());
    // arg[2] = NULL;
    // // int tmp = dup(fds[index].fd);
    // // int tmp2 = dup(1);
    // int pid = fork();
    // if (pid == -1)
    //     throw "fork error";
    // if (pid == 0)
    // {
    //     if (dup2(fds[index].fd, 1) == -1)
    //         printf("dd\n");
        
    //     close(fds[index].fd);
    //     execve(path,arg,env);
    //     close(STDOUT_FILENO);
    //     exit(0);
    // }

    // generateCgiEnv(request);
    // else
    // {
    //     waitpid(pid, &status, 0);
    // }
    
}