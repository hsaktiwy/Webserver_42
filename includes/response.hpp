/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:15:50 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/03/24 01:04:50 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include "request.hpp"
#include "http.server.hpp"

class response {
	private:
		std::string					http_response;
		std::string					body_string;
		std::string					file;
		request						*http_request;
		Worker						*worker;
		size_t						header_index;
		size_t						body_index;
		size_t						FileIndex;
		size_t						FileEnd;
		std::string					FileType;
		long long 					header_size;
		long long 					body_size;
		bool						header_sent;
		bool						body_sent;
		bool						FileOpened;
		bool						FileSeeked;
		size_t						Seeker;
		int							fd;
		bool						readyToResponed;
		bool						POST_Init;
		std::string 				boundary;
		bool						StoringFile;
		std::string					CurrentFilename;
		std::vector<std::string>	files;
		bool	PostInit(std::map<unsigned int, std::string> &status_codes, request &req, std::string &body);
		bool	PostFilesOpen(std::map<unsigned int, std::string> &status_codes, request &req, std::string &UploadPath);
		bool	PostFileFilling(std::map<unsigned int, std::string> &status_codes, request &req, std::string &body);
		void	PostResponse(std::map<unsigned int, std::string> &status_codes);
	public:
		response();
		response(request &http_request, Worker& worker);
		~response();
		response(const response& copy);
		response&   operator=(const response& obj);
		void        responed(std::map<unsigned int, std::string> &status_codes);
		void        errorresponse(std::map<unsigned int, std::string> &status_codes);
		void        RedirectionResponse(std::map<unsigned int, std::string> &status_codes, std::string &path);
		std::string Status(unsigned int status, std::map<unsigned int, std::string> &status_codes);
		void		Get(std::map<unsigned int, std::string> &status_codes);
		void		Post(std::map<unsigned int, std::string> &status_codes);

		std::string	getHttp_response( void ) const;
		std::string	getBody_string( void ) const;
		std::string	getFile( void ) const;
		size_t		getHeader_index( void ) const;
		long long	getHeader_size( void ) const;
		long long	getBody_size( void ) const;
		size_t		getBody_index( void ) const;
		bool		getHeader_sent( void ) const;
		bool		getBody_sent( void ) const;
		void		setHeader_index(size_t);
		void		setHeader_size(long long );
		void		setBody_size(long long );
		void		setBody_index(size_t );
		void		setHeader_sent(bool );
		void		setBody_sent( bool );
		bool		getFileOpened() const;
		int			getFd() const;
		std::string	getFileType( void ) const;
		size_t		getFileIndex( void ) const;
		size_t		getFileEnd( void ) const;
		bool		getFileSeeked( void ) const;
		size_t		getSeeker( void ) const;
		bool		getReadyToResponed( void ) const;


		void		setFileSeeked(bool value);
		void		setSeeker(size_t value);
		void		setFileIndex(size_t value);
		void		setFileEnd(size_t value);
		void		setWorker(Worker &worker);
		void		setHttp_request(request &request);
		void		setFileOpened( bool value );
		void		setFd( int value );

};
std::string			mime_types(std::string &extension);
#endif