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
		std::string	response(std::map<unsigned int, std::string> &status_codes);
		const Worker &getWorker( void ) const;
};

#endif