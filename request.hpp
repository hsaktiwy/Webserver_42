#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
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

    public:
        request();
        request(char *request);
        ~request();
        request(const request& copy);
        request& operator=(const request& obj);
        Method getMethod( void ) const; // to get the method when we need it
};
#endif