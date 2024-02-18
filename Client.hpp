/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:16:04 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/02/16 17:03:02 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "request.hpp"
// #include "WorkerInit.hpp"
#include "cgi/cgi.hpp"
#include "cgi/cgiResponse.hpp"
#include "response.hpp"
#include "http.server.hpp"
class Client {
	private:
		Worker		worker;
		request		http_request;
		response	http_response;
		long		time;
		bool		inProcess;
		bool		requestReceived;
		bool		responseSent;
		int			socket;
		CgiEnv      cgiRequest;
		CgiResponse cgiResponse;
	public:
		Client();
		~Client();
		Client(const Client& copy);
		Client& operator=(const Client& obj);
		void			setClientSocket(int fd);
		void			setClientRequestState(bool state);
		void			setClientResponseState(bool state);
		int				getClientSocket() const;
		bool			getClientResponseSate() const;
		bool			getClientRequestSate() const;
		long			getTime( void ) const;
		bool			getInProcess( void ) const;
		// this function will int http_request and worker plus parse the request
		void			ParseRequest(std::vector<ServerBlocks> &serverBlocks);
		void			CreateResponse(std::map<unsigned int, std::string> &status_codes);
		void			BufferingRequest(std::vector<ServerBlocks> &serverBlocks, char *buff, size_t bytes);
		response const	&getHttp_response( void ) const;
		request const	&getHttp_request( void ) const;
		Worker const	&getWorker( void ) const;
		CgiResponse  &getcgiResponse();
		CgiEnv  &getcgiRequest();
		
		void			setTime(long value);
		void			setInProcess(bool value);
};

#endif