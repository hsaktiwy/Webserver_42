/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_errors.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaktiwy <hsaktiwy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 12:17:44 by adardour          #+#    #+#             */
/*   Updated: 2024/02/07 22:34:28 by hsaktiwy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

void    proccess_error_page(std::vector<std::string> error_page_token, int line)
{
    int size = error_page_token.size() - 1;
    int i = 0;
    std::string error;
    while(i < size)
    {
        if (atoi(error_page_token[i].c_str()) < 300 || atoi(error_page_token[i].c_str()) > 599)
        {
            error = "value \"" + error_page_token[i] + "\" must be between 300 and 599 " + convertToString(line);
            throw error;
        }
        for (size_t j = 0; j < error_page_token[i].size(); j++)
        {
            if (!std::isdigit(error_page_token[i][j]))
            {
                error = "invalid value \"" + error_page_token[i] + "\" in " + convertToString(line);
                throw error;
            }
        }
        i++;
    }
}

void    handle_directives(std::string &type, std::string &directive,std::string &token,int *argument,int *is_directive,int *is_not_semi_colone,int line)
{
    std::string error;
    static int number_of_args = 0;
    static std::vector<std::string> error_page_token;
    if (!type.compare("argument"))
    {
        if (!directive.compare("uploads"))
        {   
            number_of_args++;
            if (number_of_args > 1 || number_of_args < 0)
            {
                error = "invalid number of arguments in " + directive + " directive in " + convertToString(line);
                throw error;
            }
        }
        if (!directive.compare("error_page"))
        {
            number_of_args++;
            error_page_token.push_back(token);
        }
        if (!directive.compare("autoindex"))
        {
            if (token.compare("on") && token.compare("off"))
            {
                error = "invalid value \"" + token + "\" in  \"autoindex\" directive must be \"on\" or \"off\" in " + convertToString(line);
                throw error;
            }
        }
        if (!directive.compare("allow_methods"))
        {
            if (token.compare("POST") && token.compare("GET") && token.compare("DELETE"))
            {
                error = "not a valid method \"" + token + "\" directive in " + convertToString(line);
                throw error;
            }
        }
        *argument = 1;
    }
    if (!type.compare("semi_colon"))
    {
        if (!(*argument)  || (number_of_args <= 1 && !directive.compare("error_page")))
        {
            error = "invalid number of arguments in " + directive + " directive in " + convertToString(line);
            throw error;
        }
        proccess_error_page(error_page_token, line);
        *is_directive = 0;
        *is_not_semi_colone = 1;
        *argument = 0;
        number_of_args = 0;
        error_page_token.clear();
    }
    else if (!type.compare("directive") || !type.compare("block"))
    {
        error = "invalid parameter " + token + " " + convertToString(line);
        throw error;
    }
    else if ((!type.compare("close_block") && !is_not_semi_colone))
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

void    handle_errors(tokens_map tokens)
{
    vectors_type token_vectors;
    std::string token;
    std::string type;
    std::string error;

    int insideServerBlock = 0;
    int is_not_semi_colone = 0;
    int is_location_block = 0;
    int hasPath = 0;
    int is_directive = 0;
    int opening = 0;
    int number_of_path = 0;
    int argument = 0;
    int listen_directive = 0;


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
                handle_directives(type,directive, token, &argument,&is_directive, &is_not_semi_colone,line);
            else
            {
                if (!type.compare("block"))
                {
                    if (!token.compare("server"))
                    {
                        if (insideServerBlock)
                        {
                            error = "server directive is not allowed here in " + convertToString(line);
                            throw error;
                        }
                        insideServerBlock = 1;
                    }
                    else
                    {
                        if  (!insideServerBlock || is_location_block)
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
                    if (!insideServerBlock)
                    {
                        error = "unexpected \"" + token + "\"" + " in line "  + convertToString(line);
                        throw error;
                    }
                    if (is_location_block)
                    {
                        if (number_of_path > 1 || !hasPath)
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
                        hasPath = 0;
                        number_of_path = 0;
                    }
                    else if (insideServerBlock)
                    {
                        insideServerBlock = 0;
                        is_not_semi_colone = 0;
                        is_location_block = 0;
                        hasPath = 0;
                        is_directive = 0;
                        opening = 0;
                        number_of_path = 0;
                    }
                    closed.pop();
                }
                else if (!type.compare("directive"))
                {
                    directive = token;
                    if (!directive.compare("listen"))
                        listen_directive = 1;
                    if (!directive.compare("listen") && is_location_block)
                    {
                        error = directive +  " directive is not allowed here in " + convertToString(line);
                        throw error;
                    }
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
                    std::string::iterator it = std::find(token.begin(), token.end(),'?');

                    if (it != token.end())
                    {
                        error = "reserved symbols in uri " + convertToString(line);
                        throw error;
                    }
                    hasPath = 1;
                    number_of_path++;
                }
            }
            it_v++;
        }
    }
    if (listen_directive == 0)
    {
        std::string error = "config file must include listen directive";
        throw error;
    }
    if (insideServerBlock)
    {
        std::string error = "unexpected end of file, expecting \";\" or \"}\" in line " + convertToString(line);
        throw error;
    }
    if (!closed.empty())
    {
        error = "correct your config file in " + convertToString(line);
        throw error;
    }
}

bool hasDuplicate(std::vector<std::string>& strings)
{
    std::vector<std::string> duplicate;    
    for (int i = 0; i < strings.size(); i++)
    {
        std::vector<std::string>::iterator it = std::find(duplicate.begin(), duplicate.end(), strings[i].c_str());
        if (it == duplicate.end())
        {
            duplicate.push_back(strings[i]);
        }
        else
        {
            return true;
        }
    }
    return false;
}

template<typename T>
void    handle_duplication_block(T &block)
{
    std::map<std::string, int> directiveCounts;
    std::string error;
    std::vector<Directives>  directives = block.getDirectives(); 
    for (size_t j = 0; j < directives.size(); ++j)
    {
        const std::string& directiveName = directives[j].getDirective();
        if (directiveName.compare("error_page"))
        {
            int& count = directiveCounts[directiveName];
            ++count;
        }
    }
    for (std::map<std::string, int>::const_iterator it = directiveCounts.begin(); it != directiveCounts.end(); ++it)
    {
        if (it->second > 1)
        {
            error = "theres some duplication in directive \"" + it->first + "\"";
            throw error;
        }
    }
    directiveCounts.clear();
}

void check_duplications(std::vector<ServerBlocks> serverBlocks)
{
    for (size_t i = 0; i < serverBlocks.size(); ++i)
    {
        handle_duplication_block(serverBlocks[i]);
        for (size_t j = 0; j < serverBlocks[i].getLocations().size(); j++)
        {
            LocationsBlock location = serverBlocks[i].getLocations()[j];
            handle_duplication_block(location);
        }
    }
}


