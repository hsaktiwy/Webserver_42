/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.server.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 12:03:00 by adardour          #+#    #+#             */
/*   Updated: 2023/12/08 15:58:13 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <string.h>
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <list>
#include <sstream>
#include <string>
#include <iterator>
#include <algorithm> 
#include <map> 
#include <vector>
#include <string>

typedef std::vector<std::pair<std::string, std::string> > vectors_type;
typedef std::multimap<int, std::vector<std::pair<std::string, std::string> > > tokens_map;
typedef std::vector<std::pair<std::string, int> > tokens_iterator;


class Directives
{
    private:
        std::string directive;
        std::vector<std::string> arguments;
    public:
        Directives () {};

        Directives(std::string &dir): directive(dir){};
        
        void    setArguments(std::string argument)
        {
            arguments.push_back(argument);
        }
        
        std::vector<std::string> &getArgument()
        {
            return this->arguments;
        }
        std::string getDirective() const
        {
            return directive;
        }
};


class LocationsBlock
{
    private:
        std::string path;
        std::vector<Directives> directives;
        std::stack<LocationsBlock*> nestedLocations;
    public:
        LocationsBlock()
        {
            
        };
        LocationsBlock(std::string &path)
        {
            this->path = path;
        };
        void    AddDirective(Directives &directive)
        {
            directives.push_back(directive);
        }
        std::string& getPath()
        {
            return this->path;
        }
        std::vector<Directives> &getDirective()
        {
            return this->directives;
        }
        void    setPath(std::string &path)
        {
            this->path = path;
        }
        void AddNestedLocation(LocationsBlock& nestedLocation) {
            nestedLocations.push(&nestedLocation);
        }

        LocationsBlock& GetLastNestedLocation() {
            return *nestedLocations.top();
        }
};

class ServerBlocks
{
    private:
        std::vector<LocationsBlock> locations;
        std::vector<Directives> directives;
    public:
        ServerBlocks() {
        
        };
        void    AddDirective(Directives &directive)
        {
            directives.push_back(directive);
        }
        void    AddLocation(LocationsBlock &location)
        {
            locations.push_back(location);
        }
        std::vector<LocationsBlock> & getLocations()
        {
            return this->locations;
        }
        std::vector<Directives> &getDirectives()
        {
            return this->directives;
        }
};

void    parse_line(const std::string &line,  tokens_map &tokens, int line_number);
void    parse_config(tokens_iterator  &lines);
void    print_tokens(vectors_type &tokens);
void    proccess_tokens(vectors_type &tokens);
void    print_args(std::vector<std::string> &args);
void    print_dir(std::vector<Directives> &dir);
void    print_server(std::vector<ServerBlocks> &serverBlocks);
void    getarguments(vectors_type::iterator &it,Directives &directive);
void    proccess_tokens(tokens_map &tokens);
void    print_location(std::vector<LocationsBlock> &locations);




#endif