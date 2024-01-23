#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "request.hpp"
// #include "WorkerInit.hpp"
#include "response.hpp"
class Client {
	private:
		Worker worker;
		request http_request;
		response http_response;
	public:
		Client();
		~Client();
		Client(const Client& copy);
		Client& operator=(const Client& obj);
		// this function will int http_request and worker plus parse the request
		void	ParseRequest(char *buffer, std::vector<ServerBlocks> &serverBlocks);
		void	CreateResponse(std::map<unsigned int, std::string> &status_codes);
		response const	&getHttp_response( void ) const;
		request const	&getHttp_request( void ) const;
		Worker const	&getWorker( void ) const;
};

#endif