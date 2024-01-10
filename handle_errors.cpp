/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_errors.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 12:17:44 by adardour          #+#    #+#             */
/*   Updated: 2024/01/10 14:45:34 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

const std::string convertToString(long long line)
{
    std::ostringstream convert;
    convert << line;

    return convert.str();
}

void    handle_errors(tokens_map tokens)
{
    vectors_type token_vectors;
    std::string token;
    std::string type;
    std::string error;

    int is_server_block = 0;
    int is_not_semi_colone = 0;
    int is_location_block = 0;
    int is_path = 0;
    int is_directive = 0;
    int opening = 0;
    int number_of_path = 0;
    std::string directive;

    std::stack<std::string> closed;
    long long line = 0;
    
    for (tokens_map::iterator it = tokens.begin(); it != tokens.end(); it++)
    {
        token_vectors = it->second;
        line = it->first;
        vectors_type::iterator it_v = token_vectors.begin();
        vectors_type::iterator ite_v = token_vectors.end();
        while (it_v != ite_v)
        {   
            token = it_v->first;
            type = it_v->second;
            if (!type.compare("unknown"))
            {
                error = "unknown directive \"" + token + "\" in " + convertToString(line);
                throw error;
            }
            else if (is_directive)
            {
                if (!type.compare("semi_colon"))
                {
                    is_directive = 0;
                    is_not_semi_colone = 1;
                }
                else if (!type.compare("directive") || !type.compare("block"))
                {
                    error = "invalid parameter " + token + " " + convertToString(line);
                    throw error;
                }
                else if (!type.compare("close_block") && !is_not_semi_colone)
                {
                    error = "unexpected } " + convertToString(line);
                    throw error;
                }
                else if (!type.compare("directive") || !type.compare("block"))
                {
                    if (!type.compare("block"))
                    {
                        if (!token.compare("location") && !is_not_semi_colone)
                        {
                            error = "directive " + directive + " is not terminated by ; " + convertToString(line);
                            throw error;
                        }
                    }
                    else if (!type.compare("directive") && !is_not_semi_colone)
                    {
                        error = "directive " + directive + " is not terminated by ; " + convertToString(line);
                        throw error;
                    }
                }
            }
            else
            {
                if (!type.compare("block"))
                {
                    if (!token.compare("server"))
                    {
                        if (is_server_block)
                        {
                            error = "server directive is not allowed here in " + convertToString(line);
                            throw error;
                        }
                        is_server_block = 1;
                    }
                    else
                    {
                        if  (!is_server_block)
                        {
                            error = "location directive is not allowed here in " + convertToString(line);
                            throw error;
                        }
                        is_location_block = 1;
                        number_of_path = 0;
                    }
                }
                else if (!type.compare("open_block"))
                {
                    if (is_location_block)
                    {
                        if (number_of_path > 1 || !is_path)
                        {
                            error = "invalid number of arguments in location directive in " + convertToString(line);
                            throw error;
                        }
                        opening = 1;
                    }
                    else if (!closed.empty() && !is_location_block)
                    {
                        error = "unexpected } in " + convertToString(line);
                        throw error;
                    }
                    closed.push("{");
                }
                else if (!type.compare("close_block"))
                {
                    if (closed.empty())
                    {
                        printf("unexpected } %lld",line);
                        exit(1);
                    }
                    else if (is_location_block)
                    {
                        if (!opening)
                        {
                            error = "directive location has no opening { " + convertToString(line);
                            throw error;
                        }
                        is_location_block = 0;
                        opening = 0;
                        is_path = 0;
                        number_of_path = 0;
                    }
                    else if (is_server_block)
                    {
                        is_server_block = 0;
                        is_not_semi_colone = 0;
                        is_location_block = 0;
                        is_path = 0;
                        is_directive = 0;
                        opening = 0;
                        number_of_path = 0;
                    }
                    closed.pop();
                }
                else if (!type.compare("directive"))
                {
                    directive = token;
                    if (closed.empty() || (!opening && is_location_block))
                    {
                        error = "unexpected } in " + convertToString(line);
                        throw error;
                    }
                    is_directive = 1;
                    is_not_semi_colone = 0;
                }
                else if (!type.compare("semi_colon"))
                {
                    if (!is_directive)
                    {
                        error = "unexpected ; in " + convertToString(line);
                        throw error;
                    }
                    else if (is_location_block && !opening)
                    {
                        error = "directive location has no opening { " + convertToString(line);
                        throw error;
                    }
                    is_not_semi_colone = 1;
                }
                else if (!type.compare("path"))
                {
                    is_path = 1;
                    number_of_path++;
                }
            }
            it_v++;
        }
    }
    if (!closed.empty())
    {
        error = "correct your config file in " + convertToString(line);
        throw error;
    }
}