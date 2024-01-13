/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.server.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 12:03:00 by adardour          #+#    #+#             */
/*   Updated: 2024/01/13 21:37:33 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#define FULL_PATH_PHP "/usr/bin/php"


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
#include <stack>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>


const std::string DEFAULT_CONFIG_PATH = "/Users/adardour/Desktop/www/configs/default.conf";

typedef struct 
{
    unsigned int port;
    struct in_addr sin_addr;
}   t_port_host;

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
        const std::string& getPath() const
        {
            return this->path;
        }
        std::vector<Directives> &getDirectives()
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
        std::vector<LocationsBlock> const & getLocations() const
        {
            return this->locations;
        }
        std::vector<Directives> &getDirectives()
        {
            return this->directives;
        }
};


#include <string>

class Worker {
private:
    ServerBlocks blockworker;
    LocationsBlock locationworker;
    std::string root;
    std::string index;
    std::string redirect;
    std::string error_page;
    std::string host;
    std::string path;

public:
    Worker()
    {
        
    }
    Worker(std::vector<ServerBlocks> &blocks,std::string &host);
    ServerBlocks getBlockWorker() const
    {
        return blockworker;
    }

    void setBlockWorker(const ServerBlocks& blocks)
    {
        blockworker = blocks;
    }

    LocationsBlock getLocationWorker() const
    {
        return locationworker;
    }

    void setLocationWorker(const ServerBlocks& block,std::string &path);
    std::string getRoot() const {
        return root;
    }

    void setRoot(const std::string& newRoot) {
        root = newRoot;
    }

    std::string getIndex() const {
        return index;
    }

    void setIndex(const std::string& newIndex) {
        index = newIndex;
    }

    std::string getRedirect() const {
        return redirect;
    }

    void setRedirect(const std::string& newRedirect) {
        redirect = newRedirect;
    }

    std::string getErrorPage() const {
        return error_page;
    }

    void setErrorPage(const std::string& newErrorPage) {
        error_page = newErrorPage;
    }

    std::string getHost() const {
        return host;
    }

    void setHost(const std::string& newHost) {
        host = newHost;
    }

    std::string getPath() const {
        return path;
    }

    void setPath(const std::string& newPath) {
        path = newPath;
    }
};


void    start_listening_and_accept_request(std::vector<ServerBlocks> &serverBlocks);
void        parse_line(const std::string &line,  tokens_map &tokens, int line_number);
void        parse_config(tokens_iterator  &lines, tokens_map &tokens);
void        print_tokens(vectors_type &tokens);
void        print_args(std::vector<std::string> &args);
void        print_dir(std::vector<Directives> &dir);
void        print_server(std::vector<ServerBlocks> &serverBlocks);
void        getarguments(vectors_type::iterator &it,Directives &directive);
void        proccess_tokens(tokens_map &tokens,std::vector<ServerBlocks> &serverBlocks);
void        print_location(std::vector<LocationsBlock> &locations);
void        handle_errors(tokens_map tokens);
void        print_tokens(std::multimap<int,std::vector<std::pair<std::string, std::string> > > &tokens);
std::string getTokenType(const std::string& token);
std::string trim(const std::string& str);
ServerBlocks get_server_block(std::string &host,std::vector<ServerBlocks> &serverBlocks);
// std::string get_index(std::vector<Directives> &directives);
// std::string get_root(std::vector<Directives> &directives);
std::string check_root(ServerBlocks &block);
std::string&    parse_request(char buffer[1024],std::vector<ServerBlocks> &serverBlocks,std::string &response,int *flag,int *status);
void    build_response(std::string index_path,std::string &response);
#endif