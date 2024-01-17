/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 12:44:24 by adardour          #+#    #+#             */
/*   Updated: 2024/01/16 21:30:35 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

std::string trimString(const std::string& str)
{
    size_t start = 0;
    size_t end = str.length();

    while (start < end && str[start] == ' ')
    {
        start++;
    }
    while (end > start && str[end - 1] == ' ')
    {
        end--;
    }
    return str.substr(start, end - start);
}

void    getarguments(std::vector<std::pair<std::string, std::string> >::iterator &it,Directives &directive)
{
    std::string type;
    std::string token;

    type = it->second;
    while (type.compare("semi_colon"))
    {
        token = it->first;
        directive.setArguments(token);
        it++;
        type = it->second;
    }
}
void    print_(std::vector<std::pair<std::string, std::string> > &tokens_vector)
{
    std::vector<std::pair<std::string, std::string> >::iterator it_vec = tokens_vector.begin();
    std::vector<std::pair<std::string, std::string> >::iterator ite_vec = tokens_vector.end();

    while (it_vec != ite_vec)
    {
        printf("token %s type %s\n",(*it_vec).first.c_str(),(*it_vec).second.c_str());
        it_vec++;
    }
    
}

void    print_tokens(std::multimap<int,std::vector<std::pair<std::string, std::string> > > &tokens)
{
    std::multimap<int, std::vector<std::pair<std::string, std::string> > >::iterator it = tokens.begin();
    std::multimap<int, std::vector<std::pair<std::string, std::string> > >::iterator ite = tokens.end();
    std::vector<std::pair<std::string, std::string> > tokens_vector;
    std::stack<std::string> closed;
    
    int value;
    while (it != ite)
    {
        tokens_vector = it->second;
        value = it->first; 
        print_(tokens_vector);
        it++;
    }
}

void    parse_config(tokens_iterator  &lines, tokens_map &tokens)
{
    tokens_iterator::iterator it = lines.begin();
    tokens_iterator::iterator ite = lines.end();

    while (it != ite)
    {
        parse_line((*it).first, tokens, (*it).second);
        it++;
    }
}

void    clear_token(const std::string &str, std::string &result)
{
    for (size_t i = 0; i < str.length(); ++i) {
        if ((str[i] == '{' || str[i] == '}' || str[i] == ';' || str[i] == '\n' || str[i] == '\"' || str[i] == '\'') &&
            (i == 0 || str[i - 1] != ' ')) {
            result += ' ';
        }
        result += str[i];
        if ((str[i] == '{' || str[i] == '}' || str[i] == ';' || str[i] == '\n' || str[i] == '\"' || str[i] == '\'') &&
            (i == str.length() - 1 || str[i + 1] != ' ')) {
            result += ' ';
        }
    }
}


std::string getTokenType(const std::string& token)
{

    std::map<std::string, std::string> tokenTypes;
    tokenTypes.insert(std::make_pair("server", "block"));
    tokenTypes.insert(std::make_pair("location", "block"));
    tokenTypes.insert(std::make_pair("listen", "directive"));
    tokenTypes.insert(std::make_pair("client_max_body_size", "directive"));
    tokenTypes.insert(std::make_pair("error_page", "directive"));
    tokenTypes.insert(std::make_pair("autoindex", "directive"));
    tokenTypes.insert(std::make_pair("allow_methods", "directive"));
    tokenTypes.insert(std::make_pair("index", "directive"));
    tokenTypes.insert(std::make_pair("access_log", "directive"));
    tokenTypes.insert(std::make_pair("error_log", "directive"));
    tokenTypes.insert(std::make_pair("root", "directive"));
    tokenTypes.insert(std::make_pair("to", "directive"));
    tokenTypes.insert(std::make_pair("cgi", "directive"));
    tokenTypes.insert(std::make_pair("server_name", "directive"));
    tokenTypes.insert(std::make_pair("{", "open_block"));
    tokenTypes.insert(std::make_pair("}", "close_block"));
    tokenTypes.insert(std::make_pair(";", "semi_colon"));
    tokenTypes.insert(std::make_pair("\'", "single_quote"));
    tokenTypes.insert(std::make_pair("\"", "double_quote"));
    
    std::map<std::string, std::string>::iterator it = tokenTypes.find(token);

    if (it != tokenTypes.end())
    {
        return it->second;
    } else {
        return "unknown";
    }
}


void tokenize(std::string &token, tokens_map &tokens, int line_number)
{

    static int flag = 0;
    static int flag_location = 0;
    vectors_type tokens_vec;

    if (!token.compare("\n"))
    {
        flag = 0;
        flag_location = 0;
        return;
    }
    else if (!token.compare(";"))
    {
        tokens_vec.push_back(std::make_pair(token, getTokenType(token)));
        flag = 0;
    }
    else if (flag_location == 1 && token.length() >= 1)
    {
        if (!token.compare("{")) {
            tokens_vec.push_back(std::make_pair(token, getTokenType(token)));
            flag_location = 0;
        } else {
            tokens_vec.push_back(std::make_pair(token, "path"));
        }
    } else if (flag == 1 && token.length() >= 1) {
        if (!token.compare(";")) {
            tokens_vec.push_back(std::make_pair(token, getTokenType(token)));
            flag = 0;
        } else {
            if (!token.compare("\'") || !token.compare("\"") )
            {
                tokens_vec.push_back(std::make_pair(token, getTokenType(token)));
                
            }
            else
            {
                tokens_vec.push_back(std::make_pair(token, "argument"));
            }
        }
    } else {
        if (token.length() >= 1)
        {
            std::string type = getTokenType(token);
            tokens_vec.push_back(std::make_pair(token, type));
            if (!type.compare("directive"))
            {
                flag = 1;
            } else if (!type.compare("block") && !token.compare("location"))
            {
                flag_location = 1;
            }
        }
    }
    tokens.insert(std::make_pair(line_number, tokens_vec));
}

void    parse_line(const std::string &line, tokens_map &tokens, int line_number)
{
    std::string result;
    clear_token(line, result);
    char *line_copy = new char[result.size() + 1];
    strcpy(line_copy, result.c_str());
    char *token = strtok(line_copy, " ");
    while (token != NULL)
    {
        std::string trim = trimString(token);
        tokenize(trim, tokens, line_number);
        token = strtok(NULL, " ");
    }
}
