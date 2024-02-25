/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   proccess_tokens.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaktiwy <hsaktiwy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/07 10:23:31 by adardour          #+#    #+#             */
/*   Updated: 2024/02/07 22:07:02 by hsaktiwy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

void proccess_tokens(tokens_map &tokens,std::vector<ServerBlocks> &serverBlocks)
{

    std::stack<std::string> closed;
    std::vector<LocationsBlock> locationblock;
    int inside_location_block = 0;
    int directive = 0;
    std::string token;
    std::string type;
    vectors_type vectors;

    for (tokens_map::iterator it = tokens.begin(); it != tokens.end(); it++)
    {
        vectors = it->second;
        vectors_type::iterator it_v = vectors.begin();
        vectors_type::iterator ite_v = vectors.end();
        while (it_v != ite_v)
        {   
            token = it_v->first;
            type = it_v->second;
            if (!type.compare("block"))
            {
                if (!token.compare("server"))
                {
                    serverBlocks.push_back(ServerBlocks());   
                }
                else if (!token.compare("location"))
                {
                    inside_location_block = 1;
                    locationblock.push_back(LocationsBlock());
                }
            }
            else if (directive)
            {
                if (!type.compare("argument") && !inside_location_block)
                {
                    serverBlocks.back().getDirectives().back().setArguments(token);
                }
                else if (!type.compare("argument") && inside_location_block)
                {
                    locationblock.back().getDirectives().back().setArguments(token);
                    
                }
                else if (!type.compare("semi_colone"))
                {
                    directive = 0;
                    
                }
            }
            if (!type.compare("open_block"))
            {
                closed.push("{");
            }
            else if (!type.compare("close_block"))
            {
                closed.pop();
                if (inside_location_block)
                {
                    inside_location_block = 0;
                    serverBlocks.back().AddLocation(locationblock.back());
                }
            }
            else if (!type.compare("directive"))
            {
                directive = 1;
                Directives dir(token);
                if (!inside_location_block)
                {
                    serverBlocks.back().AddDirective(dir);
                }
                else
                {
                    locationblock.back().AddDirective(dir);   
                }
            }
            else if (inside_location_block && !type.compare("path"))
            {
                locationblock.back().setPath(token);   
            }
            it_v++;
        }
    }
    if (!closed.empty() || serverBlocks.size() == 0)
    {
        std::cout << "end of file or not close something\n";
        exit(0);
    }
}