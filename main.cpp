/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 12:03:40 by adardour          #+#    #+#             */
/*   Updated: 2023/12/06 18:43:15 by adardour         ###   ########.fr       */
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
        std::vector<std::string> lines;
        if (c == 1)
            path = "default";
        std::ifstream file(path);
        if (file.is_open())
        { 
            std::string line;
            while (std::getline(file, line))
            { 
                lines.push_back(line);
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