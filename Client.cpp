/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:16:02 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/02/24 23:42:21 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


Client::Client() : http_response(http_request, worker) , cgiRequest(worker)
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
		cgiRequest = obj.cgiRequest;
		cgiRequest.setCgiWorker(obj.worker);
		cgiResponse = obj.cgiResponse;
		inProcess = obj.inProcess;
		time = obj.time;
	}
	return (*this);
}

void	Client::ParseRequest(std::vector<ServerBlocks> &serverBlocks)
{
    http_request.CheckRequest(serverBlocks, worker);
	if (http_request.getCgiStatus()) // check if the request is a cgi request
	{
		cgiRequest.setCgiWorker(worker); //set the worker of the cgi request
		cgiRequest.setRequest(this->getHttp_request().getMethod()); //init the method 
		if (!this->getHttp_request().getMethod().compare("POST"))
		{
			cgiRequest.setRequestBody(this->getHttp_request().getBody());
			std::string boundary = this->getHttp_request().getBoundary();
			if (boundary.size())
				cgiRequest.setBoundry(this->getHttp_request().getBoundary());
			std::string content_type, content_length;
			http_request.getHeaderValue("Content-Type", content_type);
			http_request.getHeaderValue("Content-Length", content_length);
			cgiRequest.setContentType(content_type);
			cgiRequest.setContentLength(content_length);
		}
			
		cgiRequest.setEnvironementData(); // fill the map of the env needed by the script process and check errors
	}
		
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

CgiResponse &Client::getcgiResponse()
{
	return cgiResponse;
}
CgiEnv &Client::getcgiRequest()
{
	return cgiRequest;
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
