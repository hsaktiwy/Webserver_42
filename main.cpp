/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 12:03:40 by adardour          #+#    #+#             */
/*   Updated: 2023/12/14 13:54:57 by adardour         ###   ########.fr       */
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

    std::ifstream file(path);
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

int main(int c,char **argv)
{
    tokens_iterator lines;
    tokens_map tokens;

    try
    {
        readAndParseConfig(c,argv,lines);
        parse_config(lines,tokens);
        print_tokens(tokens);
        // handle_errors(tokens);
        // proccess_tokens(tokens);
        // start_listening_and_accept_request(tokens);
    }
    catch(std::string & e)
    {
        std::cerr << e << '\n';
    }
    return (0);
}