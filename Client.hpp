/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:16:04 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/03/06 16:03:56 by adardour         ###   ########.fr       */
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
		int			fd_server;
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
		void			BufferingRequest(std::vector<ServerBlocks> &serverBlocks, char *buff,std::map<int, int> &matched_server_block  ,size_t bytes);
		response const	&getHttp_response( void ) const;
		request const	&getHttp_request( void ) const;
		Worker const	&getWorker( void ) const;
		CgiResponse  &getcgiResponse();
		CgiEnv  &getcgiRequest();
		
		void	setFdServer(int fd_server)
		{	
			this->fd_server = fd_server;
		}

		int	getFdServer()
		{
			return this->fd_server;
		}
		
		void			setTime(long value);
		void			setInProcess(bool value);
};

#endif