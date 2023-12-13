/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_errors.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 12:17:44 by adardour          #+#    #+#             */
/*   Updated: 2023/12/13 22:41:46 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

void    handle_errors(tokens_map tokens)
{
    vectors_type vectors;
    std::string token;
    std::string type;
    int is_server_block = 0;
    int is_not_semi_colone = 0;
    int is_location_block = 0;
    int is_path = 0;
    int is_directive = 0;
    int opening = 0;
    int number_of_path = 0;
    std::stack<std::string> closed;
    int line = 0;

    // std::vector<t_error> error;
    for (tokens_map::iterator it = tokens.begin(); it != tokens.end(); it++)
    {
        vectors = it->second;
        line = it->first;
        vectors_type::iterator it_v = vectors.begin();
        vectors_type::iterator ite_v = vectors.end();
        while (it_v != ite_v)
        {   
            token = it_v->first;
            type = it_v->second;
            if (!type.compare("unknown"))
            {
                printf("unknown directive %d\n",line);
                exit(1);
            }
            else if (is_directive)
            {
                if (!type.compare("semi_colon"))
                {
                    is_directive = 0;
                    is_not_semi_colone = 1;
                }
                else if (!type.compare("close_block") && !is_not_semi_colone)
                {
                    printf("error %d\n",line);
                    exit(1);
                }
                else if (!type.compare("directive") || !type.compare("block"))
                {
                    if (!type.compare("block"))
                    {
                        if (!token.compare("location") && !is_not_semi_colone)
                        {
                            printf("error %d\n",line);
                            exit(1);
                        }
                    }
                    if (!type.compare("directive") && !is_not_semi_colone)
                    {
                        printf("error %d\n",line);
                        exit(1);
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
                            printf("inside server block %d\n",line);
                            exit(1);
                        }
                        is_server_block = 1;
                    }
                    else
                    {
                        if  (!is_server_block)
                        {
                            printf("location directive is not allowed here %d\n",line);
                            exit(1);
                        }
                        is_location_block = 1;
                        number_of_path = 0;
                    }
                }
                else if (!type.compare("open_block"))
                {
                    if (is_location_block)
                    {
                        if (!is_path)
                        {
                            printf("error missing path %d\n",line);
                            exit(1);
                        }
                        else if (number_of_path > 1)
                        {
                            printf("error duplicate path %d\n",line);
                            exit(1);
                        }
                        opening = 1;
                    }
                    else if (!closed.empty() && !is_location_block)
                    {
                        printf("unexpected } %d\n",line);
                        exit(1);
                    }
                    closed.push("{");
                }
                else if (!type.compare("close_block"))
                {
                    if (closed.empty())
                    {
                        printf("unexpected } %d\n",line);
                        exit(1);
                    }
                    else if (is_location_block)
                    {
                        is_location_block = 0;
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
                    if (closed.empty() || (!opening && is_location_block))
                    {
                        printf("unexpected } %d\n",line);
                        exit(1);
                    }
                    is_directive = 1;
                    is_not_semi_colone = 0;
                }
                else if (!type.compare("semi_colon"))
                {

                    if (!is_directive)
                    {
                        printf("error duplicate semi colone at %d\n",line);
                        exit (1);
                    }
                    if (is_location_block && !opening)
                    {
                        printf("directive location has no opening { %d\n",line);
                        exit(1);
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
        printf("error\n");
        exit(1);
    }
    else
    {
        printf("ok\n");
    }
}