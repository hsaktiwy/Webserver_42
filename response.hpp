#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include "request.hpp"
#include "http.server.hpp"

class response {
    private:
        std::string http_response;
        std::string body_string;
        std::string file;
    public:
        request *http_request;
        Worker *worker;
        response();
        response(request &http_request, Worker& worker);
        ~response();
        response(const response& copy);
        response&   operator=(const response& obj);
        void        responed(std::map<unsigned int, std::string> &status_codes);
        void        errorresponse(std::map<unsigned int, std::string> &status_codes);
        std::string getHttp_response( void );
        std::string Status(unsigned int status, std::map<unsigned int, std::string> &status_codes);
        std::string getHttp_response( void ) const;
        std::string getBody_string( void ) const;
        std::string getFile( void ) const;
};
std::string mime_types(std::string &extension);
#endif