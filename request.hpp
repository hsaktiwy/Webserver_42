/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaktiwy <hsaktiwy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:15:48 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/02/13 21:56:17 by hsaktiwy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <set>
#include <iostream>
#include <list>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "http.server.hpp"
// #include <dirent.h>
// #include "WorkerInit.hpp"

// special character
#define CRLF "\r\n"
#define LWS "\r\n "
#define CR '\r'
#define SP ' '
#define HT '\t'
#define LF '\n'

// httpheader structor that willl hold the entty-header (name and values)
typedef struct t_HTTPHeader
{
	std::string				name;
	std::string				values;
	bool					error;
	int						status;

	// this info are for POTS method handling
	std::string				filename;
	std::string				Form_name;
	std::string 			boundry;
	size_t					length;
	long long				begin;// this can be delete
	long long				end;// this can be delete
} HTTPHeader;

// enum that will have the type of method used
typedef  enum{
	GET,
	POST,
	DELETE
}	Method;

typedef  enum{
	ABSOLUTE,
	RELATIVE,
	AUTHORITY
}	URI_Type;

// hier-part [ "?" query ] [ "#" fragment ]
typedef struct Uri
{
	URI_Type	type;
	std::string	scheme;//http:: 
	std::string	authority;// this is the host
	std::string	path; // our path
	std::string	query; // query
	std::string	fragment;
} t_uri;

class request {
    private:
		size_t					request_length;
		std::string				method;
		std::string				method_uri;
		t_uri					uri;
		std::string				http;
		std::string				host;
		std::vector<HTTPHeader>	headers;
		std::string				body;
		std::string				req;
		char					*header_start;
		char					*body_start;
		bool					error;
		int						status;
		int						is_dir;
		int 					is_regular;
		size_t					max_body_size;
		bool					maxBodySizeExist;
		//
		// tmp
		size_t					BIndex;
		bool					left_CR;// this can hold a incomplete dilimiter 0 noting \r will have 1 or \r\n\r
		bool					NewLine;// in case were there is \r\n
		bool					RequestRead;
		bool					ReadedFullToken;
		bool					FillingBuffer;
		// booleans to check for the Start line parsing Method URI, PROTOCOL
		bool					Parsed_StartLine;
		bool					SLValidity;
		bool					R_Method;
		bool					R_URI;
		bool					R_PROTOCOL;
		//  booleans to check for the Headerfileparsing, 
		bool					R_FUll_HEADERS;//reading full headers
		bool					Parsed_Header;
		bool					R_HEADER;
		bool					R_VALUE;
		//	Body Reading
		int						BodyLimiterType; // 1 for Content-length, 2 for chunked, 3 for boundary
		bool					R_FULL_BODY;
		bool					Body_Exist;
		bool					Parsed_Body;
		bool					ContentLengthExist;
		size_t					ContentLengthSize;
		bool					HandleRequest;
		// chunk reading
		bool					ChunkedRead;
		bool					ChunkedSizeRead;
		size_t					ChunkedSize;
		std::string				ChunkSizeString;
		// boundary case
		std::string				boundary;
    public:
		request();
		~request();
		request(const request& copy);
		// void							ParseRequest(char *request);
		void							ParseRequest(std::vector<ServerBlocks> &serverBlocks, Worker& worker,char *buff, ssize_t bytes);
		void							CheckRequest(std::vector<ServerBlocks> &serverBlocks, Worker& worker);// THIS WILL CHECK THE REQUEST VALIDITY
		request&						operator=(const request& obj);

		// Method							getMethod( void ) const; // to get the method when we need it
		// const std::vector<std::string>&	getHeaders( void ) const;// get the headers after being prased
		void							RequestDisplay( void );
		int 							getHeaderIndex(const std::string &name) const;
		void							AddToRawRequest(char *buff,  ssize_t bytes_read);
		std::string	const				&getMethod( void ) const;
		std::string	const				&getMethod_uri( void ) const;
		t_uri	const					&getUri( void ) const;
		std::string	const				&getHttp( void ) const;
		std::string	const				&getHost( void ) const;
		std::vector<HTTPHeader>	const	&getHeaders( void ) const;
		std::string	const				&getBody( void ) const;
		std::string	const				&getReq( void ) const;
		bool							getError( void ) const;
		int								getStatus( void ) const;
		int								getIs_dir( void ) const;
		int 							getIs_regular( void ) const;
		bool							getRequestRead( void ) const;
		bool							getHandleRequest( void ) const;
		size_t							getRequestLength( void ) const;

		// void							setRequestLength(bool value);
		void							setRequestRead(bool value);
		void							setHandleRequest(bool value);
		void							setError(bool value);
		void							setStatus(int value);
};
std::string get_root(std::vector<Directives> &directives, t_uri &uri);
#endif