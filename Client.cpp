#include "Client.hpp"


Client::Client()
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
	}
	return (*this);
}

void	Client::ParseRequest(char *buffer, std::vector<ServerBlocks> &serverBlocks)
{
	http_request.ParseRequest(buffer);
    http_request.CheckRequest(serverBlocks, worker);
}