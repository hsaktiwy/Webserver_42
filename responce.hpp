#ifndef RESPONCE_HPP
#define RESPONCE_HPP
#include "request.hpp"
#include "http.server.hpp"

class responce {
    private:
        std::string http_responce;
        request *http_request;
        Worker *worker;
    public:
        responce();
        responce(request &http_request, Worker& worker);
        ~responce();
        responce(const responce& copy);
        responce&   operator=(const responce& obj);
        void        responed(std::map<unsigned int, std::string> &status_codes);
        void        errorResponce(std::map<unsigned int, std::string> &status_codes);
        std::string getHttp_responce( void );
        std::string Status(unsigned int status, std::map<unsigned int, std::string> &status_codes);
};

#endif