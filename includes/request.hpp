/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:15:48 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/03/18 01:52:52 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <list>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "http.server.hpp"

#define SP ' '
#define HT '\t'

typedef struct t_HTTPHeader
{
	std::string				name;
	std::string				values;
	std::string 			boundry;
} HTTPHeader;

typedef struct Uri
{
	std::string	authority;
	std::string	path;
	std::string	query;
} t_uri;

class request {
    private:
		bool					display;
		std::string				method;
		std::string				method_uri;
		t_uri					uri;
		std::string				http;
		std::string				host;
		std::vector<HTTPHeader>	headers;
		std::string				body;
		bool					error;
		int						status;
		int						is_dir;
		int 					is_regular;
		size_t					max_body_size;
		bool					maxBodySizeExist;
		bool					is_indexDir;
		bool					redirect;
		size_t					BIndex;
		bool					left_CR;
		bool					NewLine;
		bool					RequestRead;
		bool					FillingBuffer;
		bool					Parsed_StartLine;
		bool					SLValidity;
		bool					R_Method;
		bool					R_URI;
		bool					R_PROTOCOL;
		bool					R_FUll_HEADERS;
		bool					Parsed_Header;
		bool					R_HEADER;
		bool					R_VALUE;
		int						BodyLimiterType; 
		bool					R_FULL_BODY;
		bool					Body_Exist;
		size_t					ContentLengthSize;
		bool					HandleRequest;
		bool					ChunkedSizeRead;
		size_t					ChunkedSize;
		std::string				ChunkSizeString;
		std::string				boundary;
		int					isCgiRequest;

		bool	StartlineParsing(char *buff, size_t &bytes_size, size_t &index);
		bool	MethodParsing(char *buff, size_t &bytes_size, size_t &index);
		bool	UriParsing(char *buff, size_t &bytes_size, size_t &index);
		bool	ProtocolParsing(char *buff, size_t &bytes_size, size_t &index);
		bool 	HeadersParsing(std::vector<ServerBlocks> &serverBlocks, Worker& worker, char *buff, size_t &bytes_size, size_t &index,int fd,std::map<int, int> &matched_server_block);
		bool	BaseHeadersParsing(char *buff, size_t &bytes_size, size_t &index);
		bool	IdentifieHost( void );
		void	BodyDelimiterIdentification( void );
		bool	BodyParsing(char *buff, size_t &bytes_size, size_t &index);
		bool	BodyIdentifiedByContentLength(char *buff, size_t &bytes_size, size_t &index);
		void	BodyIdentifiedByTransfertEncoding(char *buff, size_t &bytes_size, size_t &index);
		void	BodyIdentifiedByMultFormData(char *buff, size_t &bytes_size, size_t &index);

    public:
		request();
		~request();
		request(const request& copy);

		void							ParseRequest(std::vector<ServerBlocks> &serverBlocks,std::map<int, int> &matched_server_block , Worker& worker, char *buff, size_t bytes_size,int fd);
		void							CheckRequest(Worker& worker, bool &cgiStat);
		request&						operator=(const request& obj);
		bool							isRedirect( void );
		bool							isIndexDir( void );
		int 							getHeaderIndex(const std::string &name) const;
		int								getHeaderValue(const std::string &header,std::string &buffer);
		std::string	const				&getMethod( void ) const;
		std::string	const				&getMethod_uri( void ) const;
		t_uri	const					&getUri( void ) const;
		std::string	const				&getHttp( void ) const;
		std::string	const				&getHost( void ) const;
		std::vector<HTTPHeader>	const	&getHeaders( void ) const;
		std::string	const				&getBody( void ) const;
		std::string const				&getBoundary( void ) const;
		bool							getError( void ) const;
		int								getStatus( void ) const;
		int								getIs_dir( void ) const;
		int 							getIs_regular( void ) const;
		bool							getRequestRead( void ) const;
		bool							getHandleRequest( void ) const;
		int								getCgiStatus( void ) const;
		void							setRequestRead(bool value);
		void							setHandleRequest(bool value);
		void							setError(bool value);
		void							setStatus(int value);
		bool 							isCgiLocationMatched(Worker &worker);
		
		bool 							getStartLine( void ) const;
		bool							getDisplay( void ) const;
		void							setDisplay(bool value);

};
std::string get_root(std::vector<Directives> &directives, t_uri &uri);
#endif