#include "Client.hpp"


Client::Client() : http_responce(http_request, worker)
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
		http_responce = obj.http_responce;
	}
	return (*this);
}

void	Client::ParseRequest(char *buffer, std::vector<ServerBlocks> &serverBlocks)
{
	http_request.ParseRequest(buffer);
    http_request.CheckRequest(serverBlocks, worker);
}

std::string	Client::Responce(std::map<unsigned int, std::string> &status_codes)
{
	http_responce.responed(status_codes);
	return (http_responce.getHttp_responce());
}
