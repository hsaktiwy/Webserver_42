#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "request.hpp"
#include "WorkerInit.hpp"
class Client {
	private:
		Worker worker;
		request http_request;
	public:
		Client();
		~Client();
		Client(const Client& copy);
		Client& operator=(const Client& obj);
		// this function will int http_request and worker plus parse the request
		void	ParseRequest(char *buffer, std::vector<ServerBlocks> &serverBlocks);
};

#endif