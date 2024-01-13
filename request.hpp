#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <set>
#include <iostream>
#include <list>
#include <math.h>
#include "http.server.hpp"

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
    std::string             name;
    std::list<std::string>  values;
    bool                    error;
    int                     status;
} HTTPHeader;

// enum that will have the type of method used
typedef  enum{
    GET,
    POST,
    DELETE
}   Method;

typedef  enum{
    ABS,
    REL,
    AUTO
}  URI;

class request {
    private:
        std::string method;
        std::string uri;
        std::string http;
        std::string host;
        std::vector<HTTPHeader> headers;
        std::string body;
        std::string req;
        bool    error;
        int     status;
    public:
        request();
        ~request();
        request(const request& copy);
        void ParseRequest(char *request);
        void CheckRequest(std::vector<ServerBlocks> &serverBlocks);// THIS WILL CHECK THE REQUEST VALIDITY
        request& operator=(const request& obj);
        Method getMethod( void ) const; // to get the method when we need it
        const std::vector<std::string>& getHeaders( void ) const;// get the headers after being prased
        void    RequestDisplay( void );
};
#endif