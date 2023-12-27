#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <set>
#include <iostream>
#include <list>
#include <math.h>

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

class request {
    private:
        std::string method;
        std::string uri;
        std::string http;
        std::vector<HTTPHeader> headers;
        std::string req;
        bool    error;
        int     status;
        request();
    public:
        request(char *request);
        ~request();
        request(const request& copy);
        request& operator=(const request& obj);
        Method getMethod( void ) const; // to get the method when we need it
        const std::vector<std::string>& getHeaders( void ) const;// get the headers after being prased
};
#endif