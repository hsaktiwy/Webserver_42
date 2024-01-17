/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 12:03:40 by adardour          #+#    #+#             */
/*   Updated: 2024/01/17 12:50:39 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

void    readAndParseConfig(int c, char **argv,tokens_iterator &lines)
{
    int line_number = 1;

    std::string     path;
    std::string     line;
    std::ifstream   configfile;

    if (c == 1)
        path = DEFAULT_CONFIG_PATH;
    else 
        path = argv[1];
    std::ifstream file(path.c_str());
    if (file.is_open())
    { 
        std::string line;
        while (std::getline(file, line))
        { 
            lines.push_back(std::make_pair(line + '\n',line_number));
            line_number++;
        }
        file.close(); 
    }
    else
    {
        std::cout << "Unable to open file" << std::endl;
        exit(1);
    }
}

void show_info(std::vector<ServerBlocks> &blocks) {
    
    printf("\nSERVER INFO\t\n");
    for (size_t i = 0; i < blocks.size(); i++) {
        int j = 0;
        std::cout << "\x1B[1;34m";  // Set text color to blue
        printf("server #%lu\n", i);
        std::cout << "\x1B[0m";  // Reset text color

        while (j < blocks[i].getDirectives().size()) {
            if (!blocks[i].getDirectives()[j].getDirective().compare("server_name"))
            {
                std::cout << "\t\x1B[1;32m";  // Set text color to green
                printf("Server Name %s\n", blocks[i].getDirectives()[j].getArgument()[0].c_str());
                std::cout << "\x1B[0m";  // Reset text color
            }
            if (!blocks[i].getDirectives()[j].getDirective().compare("listen"))
            {
                std::cout << "\t\x1B[1;32m";  // Set text color to green
                printf("Host:Port %s\n", blocks[i].getDirectives()[j].getArgument()[0].c_str());
                std::cout << "\x1B[0m";  // Reset text color
            }
            j++;
        }
        printf("\n");
    }
    printf("\n");
}

int main(int c,char **argv)
{
    tokens_iterator lines;
    tokens_map tokens;
    std::vector<ServerBlocks> serverBlocks;

    try
    {
        readAndParseConfig(c,argv,lines);
        parse_config(lines,tokens);
        handle_errors(tokens);
        proccess_tokens(tokens,serverBlocks);
        show_info(serverBlocks);
        start_listening_and_accept_request(serverBlocks);
    }
    catch(std::string & e)
    {
        std::cerr << e << '\n';
    }
    return (0);
}