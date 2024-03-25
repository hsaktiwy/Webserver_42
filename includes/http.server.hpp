/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.server.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 12:03:00 by adardour          #+#    #+#             */
/*   Updated: 2024/03/19 22:44:06 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#define FULL_PATH_PHP "/usr/bin/php"

#include <ctime>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <list>
#include <sstream>
#include <iterator>
#include <algorithm> 
#include <map> 
#include <stack>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <netdb.h>
#include <set>
#include <signal.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define SERVERNAME "Jhin"
#define TIME_OUT 10000
#define C_TIMEOUT 10
#define UPLOADCHUNK_SIZE 1000000
#define CHUNK_SIZE 1024

const std::string DEFAULT_CONFIG_PATH = "configs/http.conf";

typedef struct 
{
    unsigned int port;
    std::string host;
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
            return this->directive;
        }
};


class LocationsBlock
{
    private:
        std::string path;
        std::vector<Directives> directives;
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
        std::vector<Directives>  &getDirectives() 
        {
            return this->directives;
        }

        void    setPath(std::string &path)
        {
            this->path = path;
        }
};

class ServerBlocks
{
    private:
        std::vector<LocationsBlock> locations;
        std::vector<Directives> directives;
    public:
        ServerBlocks()
        {
        
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

class Worker {
private:
    ServerBlocks blockworker;
    LocationsBlock locationworker;
    std::string root;
    std::string index;
    std::string redirect;
    std::string host;
    std::string path;
    std::string query;
    std::string autoindex;
    std::string max_body_size;
    std::string path_error_page;
    std::string path_upload;
    std::string cgi_python;
    std::string cgi_bash;
    std::vector<std::vector<std::string> > error_page;
    std::vector<std::string> allow_methods;
    int track_status;
    bool isCgi;


public:
    Worker();
    Worker(std::vector<ServerBlocks> &blocks,std::string &host);
    
    ServerBlocks getBlockWorker() const;
    LocationsBlock getLocationWorker() const;
    
    
    std::string getRoot() const;
    std::string get_bash_bin() const;
    std::string get_python_bin() const;
    std::string getQuery() const;
    std::string getIndex() const;
    std::string getRedirect() const;
    std::string getHost() const;
    std::string getPath() const;
    std::string    getPathError() const;
    std::string getAutoIndex() const;
    std::string &get_max_body_size();
    std::string &getPathUpload();
    std::vector<std::string> const &getAllowMethods() const;
    std::vector<std::vector<std::string> >  &getErrorPages() ;
    std::vector<std::string> const &getCgi() const ;

    void    set_bash_bin(const std::string &path);
    void    set_python_bin(const std::string &path);
    void    setBlockWorker(const ServerBlocks& blocks);
    void    setLocationWorker(const ServerBlocks& block,std::string &path);
    void    setRoot(const std::string& newRoot);
    void    setIndex(const std::vector<std::string>&   args,const std::string &root);
    void    setRedirect(const std::string& newRedirect);
    void    setMethod(std::vector<std::string>  &args);
    void    setHost(const std::string& newHost);
    void    setAutoIndex(const std::string& newPath);
    void    setPath(const std::string& newPath);
    void    setErrorPages(std::vector<std::string>  &args);
    void    set_max_body_size(std::string max_body_size);
    bool    exact_match(const ServerBlocks &block,std::string &path);
    bool    prefix_match(const ServerBlocks &block,std::string &path);
    bool    find_root(const ServerBlocks &block);
    void    found_index_file(const std::string &root);
    void    setPathError(const std::vector<std::vector<std::string> > &error_page, int status);
    void    setQuery(std::string const &query);
    void    setPathUpload(std::string const &path);
    void    set_track_status(int flag);
    void    setCgiStatus(bool state);
    bool    getCgiStatus();
    int     get_track_status();
};

void            start_listening_and_accept_request(std::vector<ServerBlocks> &serverBlocks, std::map<unsigned int, std::string> &status_codes);
void            parse_line(const std::string &line,  tokens_map &tokens, int line_number);
void            parse_config(tokens_iterator  &lines, tokens_map &tokens);
void            getarguments(vectors_type::iterator &it,Directives &directive);
void            proccess_tokens(tokens_map &tokens,std::vector<ServerBlocks> &serverBlocks);
void            handle_errors(tokens_map tokens);
std::string     getTokenType(const std::string& token);
std::string     trim(const std::string& str);
std::string     ft_trim(const std::string& str, const std::string& targets);
int             Is_Directory(const std::string &root);
void            init_worker_block(Worker &worker, std::string &host ,std::string &path,std::vector<ServerBlocks> &serverBlocks, int &is_dir, int &is_regular,int fd,std::map<int, int> &matched_server_block);
void            check_duplications(std::vector<ServerBlocks> serverBlocks);
const std::string convertToString(long long line);

bool	        ishexa(char c);
bool            check_uri(const std::string &uri);
std::string     NormilisePath(std::string &Path);
long long       ft_atoll(const char* str);
void	        ExtractValues(std::string &str, std::string  &holder, size_t &index);
long	        CurrentTime( void );

template <typename T> std::string ToString(T &data)
{
	std::string result;
	std::stringstream ss;
	ss << data;
	ss >> result;
	return (result);
}

#endif