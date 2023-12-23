#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>

// special character
#define CRLF "\r\n"
#define LWS "\r\n "
#define SP ' '
#define HT '\t'
#define LF '\n'

// enum that will have the type of method used
typedef    enum{
    GET,
    PUT,
    DELETE
}   Method;

class request {
    private:
        Method meth;
        std::vector<std::string> headers;
        std::string req;
        request();
    public:
        request(char *request);
        ~request();
        request(const request& copy);
        request& operator=(const request& obj);
        Method getMethod( void ) const; // to get the method when we need it
        const std::vector<std::string>& getHeaders( void );// get the headers after being prased
};
#endif