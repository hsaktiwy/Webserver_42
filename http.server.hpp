
#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

// typedef enum tokens_type
// {
//     server,
//     listen,
//     client_max_body_size,
//     error_page,
//     autoindex,
//     allow_methods,
//     error_log,
//     access_log,
//     index_directive,
//     semi_colon,
//     open_block,
//     close_block,
//     location
// }   tokens_type;

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
std::string get_index(std::vector<Directives> &directives);
std::string get_root(std::vector<Directives> &directives);
std::string check_root(ServerBlocks &block);
std::string&    parse_request(char buffer[1024],std::vector<ServerBlocks> &serverBlocks,std::string &response);

#endif