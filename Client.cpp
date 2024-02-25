/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaktiwy <hsaktiwy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:16:02 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/02/14 17:20:41 by hsaktiwy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


Client::Client() : http_response(http_request, worker)
{
	requestReceived = false;
	responseSent = false;
	inProcess = false;
	time = CurrentTime();
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
		socket = obj.socket;
		requestReceived = obj.requestReceived;
		responseSent = obj.responseSent;
		http_response.setHttp_request(http_request);
		http_response.setWorker(worker);
		inProcess = obj.inProcess;
		time = obj.time;
	}
	return (*this);
}

void	Client::ParseRequest(std::vector<ServerBlocks> &serverBlocks)
{
    http_request.CheckRequest(serverBlocks, worker);
}

void	Client::CreateResponse(std::map<unsigned int, std::string> &status_codes)
{
	http_response.responed(status_codes);
}

void	Client::BufferingRequest(std::vector<ServerBlocks> &serverBlocks, char *buff, size_t bytes)
{
	if (inProcess == false)
		inProcess = true;
    http_request.ParseRequest(serverBlocks, worker, buff, bytes);
}

response const	&Client::getHttp_response( void ) const
{
	return (http_response);
}

request const	&Client::getHttp_request( void ) const
{
	return (http_request);
}

const Worker &Client::getWorker( void ) const
{
	return worker;
}

void	Client::setClientSocket(int fd)
{
	this->socket = fd;
}

void	Client::setClientRequestState(bool state)
{
	this->requestReceived = state;
}

void	Client::setClientResponseState(bool state)
{
	this->responseSent = state;
}

int	Client::getClientSocket() const
{
	return this->socket;
}

bool	Client::getClientResponseSate() const
{
	return this->responseSent;
}

bool	Client::getClientRequestSate() const
{
	return this->requestReceived;

}

long			Client::getTime( void ) const
{
	return (time);
}

bool			Client::getInProcess( void ) const
{
	return (inProcess);
}

void			Client::setTime( long value)
{
	time = value;
}

void			Client::setInProcess( bool value)
{
	inProcess = value;
}