#include "Client.hpp"


Client::Client() : http_response(http_request, worker)
{

}

Client::~Client()
{

}

Client::Client(const Client& copy)
{
	*this = copy;
}

Client& Client::operator=(const Client& obj)
{
	if (this != &obj)
	{
		worker = obj.worker;
		http_request = obj.http_request;
		http_response = obj.http_response;
	}
	return (*this);
}

void	Client::ParseRequest(char *buffer, std::vector<ServerBlocks> &serverBlocks)
{
	http_request.ParseRequest(buffer);
	
    http_request.CheckRequest(serverBlocks, worker);
}

std::string	Client::response(std::map<unsigned int, std::string> &status_codes)
{
	http_response.responed(status_codes);
	return (http_response.getHttp_response());
}
const Worker &Client::getWorker( void ) const
{
	std::cout << worker.getRoot() << std::endl;
	return worker;
}
