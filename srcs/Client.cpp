/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:16:02 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/03/24 01:10:48 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client() : http_response(http_request, worker) , cgiRequest(worker)
{
	requestReceived = false;
	responseSent = false;
	inProcess = false;
	iscgi = false;
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
		this->fd_server = obj.fd_server;
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
		iscgi = obj.iscgi;
	}
	return (*this);
}
void	Client::set_cgi_status(bool val)
{
	iscgi = val;
}
bool	&Client::get_cgi_status()
{
	return iscgi;
}
void	Client::ParseRequest( void )
{
    http_request.CheckRequest(worker, this->get_cgi_status());
	if (get_cgi_status())
	{
			std::string http_cookie;
		cgiRequest.setCgiWorker(worker);
		cgiRequest.setRequest(this->getHttp_request().getMethod());
		http_request.getHeaderValue("Cookie", http_cookie);
		cgiRequest.setHttpCookies(http_cookie);
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
		else if (this->getHttp_request().getMethod().compare("GET"))
			 cgiRequest.setStatusCode(405);		
		cgiRequest.setEnvironementData();
	}	
}

void	Client::CreateResponse(std::map<unsigned int, std::string> &status_codes)
{
	http_response.responed(status_codes);
}

void	Client::BufferingRequest(std::vector<ServerBlocks> &serverBlocks, char *buff, std::map<int, int> &matched_server_block, size_t bytes)
{
	
	if (inProcess == false)
		inProcess = true;
    http_request.ParseRequest(serverBlocks, matched_server_block, worker, buff , bytes, fd_server);
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

int				Client::getFdServer( void )
{
	return fd_server;
}

void			Client::setTime( long value)
{
	time = value;
}

void			Client::setInProcess( bool value)
{
	inProcess = value;
}

void			Client::setFdServer(int fd_s)
{	
	fd_server = fd_s;
}
void			Client::setCgiResponse(CgiResponse &obj)
{
	cgiResponse = obj;
}
