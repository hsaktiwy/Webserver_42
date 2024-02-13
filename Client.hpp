/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:16:04 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/02/10 23:39:09 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		
		bool requestReceived;
		bool responseSent;
		int socket;
		
	public:
		Client();
		~Client();
		Client(const Client& copy);
		Client& operator=(const Client& obj);
		void setClientSocket(int fd);
		void setClientRequestState(bool state);
		void setClientResponseState(bool state);
		int getClientSocket() const;
		bool getClientResponseSate() const;
		bool getClientRequestSate() const;
		// this function will int http_request and worker plus parse the request
		void	ParseRequest(char *buffer, std::vector<ServerBlocks> &serverBlocks);
		void	CreateResponse(std::map<unsigned int, std::string> &status_codes);
		response const	&getHttp_response( void ) const;
		request const	&getHttp_request( void ) const;
		Worker const	&getWorker( void ) const;
};

#endif