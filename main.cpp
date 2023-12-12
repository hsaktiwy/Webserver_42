/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 12:03:40 by adardour          #+#    #+#             */
/*   Updated: 2023/12/12 11:00:41 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

int main(int c,char **argv)
{
    if (c == 2 || c == 1)
    {
        std::string path = argv[1];
        std::string line;
        std::ifstream configfile;
        tokens_iterator lines;
        if (c == 1)
            path = "default";
        int line_number = 1;
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
            parse_config(lines);
        }
        else
        {
            std::cout << "Unable to open file" << std::endl;
        }
    }
}