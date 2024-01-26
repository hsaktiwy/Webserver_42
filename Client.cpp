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
	// printf("size size     %lu\n", http_response.worker->getErrorPages().size());
}

void	Client::CreateResponse(std::map<unsigned int, std::string> &status_codes)
{
	http_response.responed(status_codes);
}

response const	&Client::getHttp_response( void ) const
{
	return (http_response);
}

request const	&Client::getHttp_request( void ) const
{
	return (http_request);
}

Worker const	&Client::getWorker( void ) const
{
	return (worker);
}
