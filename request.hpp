#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <set>
#include <iostream>
#include <list>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
// #include <dirent.h>
#include "WorkerInit.hpp"

// special character
#define CRLF "\r\n"
#define LWS "\r\n "
#define CR '\r'
#define SP ' '
#define HT '\t'
#define LF '\n'

// httpheader structor that willl hold the entty-header (name and values)
typedef struct t_HTTPHeader
{
	std::string				name;
	std::list<std::string>	values;
	bool					error;
	int						status;
} HTTPHeader;

// enum that will have the type of method used
typedef  enum{
	GET,
	POST,
	DELETE
}	Method;

typedef  enum{
	ABSOLUTE,
	RELATIVE,
	AUTHORITY
}	URI_Type;

// hier-part [ "?" query ] [ "#" fragment ]
typedef struct Uri
{
	URI_Type	type;
	std::string	scheme;
	std::string	authority;// this is the host
	std::string	path; // our path
	std::string	query; // query
	std::string	fragment;
} t_uri;

class request {
    private:
		std::string				method;
		std::string				method_uri;
		t_uri					uri;
		std::string				http;
		std::string				host;
		std::vector<HTTPHeader>	headers;
		std::string				body;
		std::string				req;
		bool					error;
		int						status;
		int						is_dir;
		int 					is_regular;
    public:
		request();
		~request();
		request(const request& copy);
		void							ParseRequest(char *request);
		void							CheckRequest(std::vector<ServerBlocks> &serverBlocks, Worker& worker);// THIS WILL CHECK THE REQUEST VALIDITY
		request&						operator=(const request& obj);
		Method							getMethod( void ) const; // to get the method when we need it
		const std::vector<std::string>&	getHeaders( void ) const;// get the headers after being prased
		void							RequestDisplay( void );
		std::string				&getMethod( void );
		std::string				&getMethod_uri( void );
		t_uri					&getUri( void );
		std::string				&getHttp( void );
		std::string				&getHost( void );
		std::vector<HTTPHeader>	&getHeaders( void );
		std::string				&getBody( void );
		std::string				&getReq( void );
		bool					getError( void );
		int						getStatus( void );
		int						getIs_dir( void );
		int 					getIs_regular( void );
		void					setError(bool value);
		void					setStatus(int value);
};
std::string get_root(std::vector<Directives> &directives, t_uri &uri);
#endif