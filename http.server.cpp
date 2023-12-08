/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 12:44:24 by adardour          #+#    #+#             */
/*   Updated: 2023/12/08 16:28:49 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

std::string trimString(const std::string& str)
{
    size_t start = 0;
    size_t end = str.length();
    while (start < end && std::isspace(str[start]))
    {
        start++;
    }
    while (end > start && std::isspace(str[end - 1]))
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
  
    int value;
    while (it != ite)
    {
        tokens_vector = it->second;
        value = it->first; 
        print_(tokens_vector);
        it++;
    }
    
    // printf("size of token %lu\n",tokens.size());
    // int i = 0;
    // while(i < tokens.size())
    // {
    //     i++;
    // }
    // std::vector<std::pair<std::string, std::string> >::iterator it = tokens.;
    // std::vector<std::pair<std::string, std::string> >::iterator ite = tokens.end();
    
    // while (it != ite)
    // {
    //     printf("Token %s ",it->first.c_str());
    //     printf("Type %s \n",it->second.c_str());
    //     it++;
    // }
}


void    parse_config(tokens_iterator  &lines)
{
    tokens_iterator::iterator it = lines.begin();
    tokens_iterator::iterator ite = lines.end();
    tokens_map tokens;

    while (it != ite)
    {
        parse_line((*it).first, tokens,(*it).second);
        it++;
    }
    proccess_tokens(tokens);
}

void    clear_token(std::string &str, std::string &result)
{
    for (size_t i = 0; i < str.length(); ++i) {
        if ((str[i] == '{' || str[i] == '}' || str[i] == ';') &&
            (i == 0 || str[i - 1] != ' ')) {
            result += ' ';
        }
        result += str[i];
        if ((str[i] == '{' || str[i] == '}' || str[i] == ';') &&
            (i == str.length() - 1 || str[i + 1] != ' ')) {
            result += ' ';
        }
    }
}

std::string gettype(std::string &token)
{
    if (token == "server")
    {
        return "block";
    } 
    else if (token == "listen")
    {
        return "directive";
    }
    else if (token == "client_max_body_size")
    {
        return "directive";
    }
    else if (token == "error_page")
    {
        return "directive";
    }
    else if (token == "autoindex")
    {
        return "directive";
    }
    else if (token == "allow_methods")
    {
        return "directive";
    }
    else if (token == "root")
    {
        return "directive";
    }
    else if (token == "index")
    {
        return "directive";
    }
    else if (token == "server_name")
    {
        return "directive";
    }
    else if (token == ";") 
    {
        return "semi_colon";
    }
    else if (token == "{") 
    {
        return "open_block";
    }
    else if (token == "}") 
    {
        return "close_block";
    } 
    else if (token == "location")
    {
        return "block";
    }
    else
    {
        return "unknown";
    }
}

void    tokenize(std::string &token,tokens_map &tokens, int line_number)
{
    std::string result;
    clear_token(token, result);
    static int flag;
    static int flag_location;
    std::istringstream iss(result);
    std::string string;
    std::string type;
    while (getline(iss, string, ' '))
    {
        vectors_type tokens_vec;
        if (flag_location == 1 && string.length() >= 1)
        {
            if(!string.compare("{"))
            {
                tokens_vec.push_back(std::make_pair(string, gettype(string)));
                flag_location = 0;
            }
            else
            {
                tokens_vec.push_back(std::make_pair(string, "path"));  
            }
        }
        else if (flag == 1 && string.length() >= 1)
        {
            if(!string.compare(";"))
            {
                tokens_vec.push_back(std::make_pair(string, gettype(string)));  
                flag = 0;
            }
            else
            {
                tokens_vec.push_back(std::make_pair(string, "argument"));  
            }
        }
        else
        {
            if (string.length() >= 1)
            {
                type = gettype(string);
                tokens_vec.push_back(std::make_pair(string, type));       
                if (!type.compare("directive"))
                {
                    flag = 1;
                }
                else if (!type.compare("block") && !string.compare("location"))
                {
                    flag_location = 1;
                }
            }
        }
        tokens.insert(std::make_pair(line_number,tokens_vec));
    }
}

void    parse_line(const std::string &line, tokens_map &tokens,int line_number)
{
    std::stringstream string_object(line);
    std::string token;
    while (std::getline(string_object, token, ' '))
    {
        if (token.length() >= 1)
        {
            std::string trim = trimString(token);
            tokenize(trim, tokens, line_number);
        }
    }
}
