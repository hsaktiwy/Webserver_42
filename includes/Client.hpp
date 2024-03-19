/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:16:04 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/03/19 22:08:27 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "request.hpp"
#include "cgi.hpp"
#include "cgiResponse.hpp"
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
		bool		iscgi;
	public:
		Client();
		~Client();
		Client(const Client& copy);
		Client& operator=(const Client& obj);
		void			ParseRequest( void );
		void			CreateResponse(std::map<unsigned int, std::string> &status_codes);
		void			BufferingRequest(std::vector<ServerBlocks> &serverBlocks, char *buff,std::map<int, int> &matched_server_block  ,size_t bytes);

		int				getClientSocket() const;
		bool			getClientResponseSate() const;
		bool			getClientRequestSate() const;
		long			getTime( void ) const;
		bool			getInProcess( void ) const;
		response const	&getHttp_response( void ) const;
		request const	&getHttp_request( void ) const;
		Worker const	&getWorker( void ) const;
		CgiResponse  	&getcgiResponse();
		CgiEnv  		&getcgiRequest();
		int				getFdServer( void );
		void			set_cgi_status(bool val);
		bool			&get_cgi_status();
		void			setClientSocket(int fd);
		void			setClientRequestState(bool state);
		void			setClientResponseState(bool state);
		void			setFdServer(int fd_s);
		void			setTime(long value);
		void			setInProcess(bool value);
		void			setCgiResponse(CgiResponse &obj);
};

#endif