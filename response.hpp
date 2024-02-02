#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include "request.hpp"
#include "http.server.hpp"

class response {
	private:
		std::string	http_response;
		std::string	body_string;
		std::string	file;
		request		*http_request;
		Worker		*worker;
		size_t		header_index;
		size_t		body_index;// this will play the role of the index where the reading start untell it reash the body size;
		size_t		FileIndex;
		size_t		FileEnd;
		std::string	FileType;
		long		long header_size;
		long		long body_size;// this play the role of the range (even the size of the full file)
		bool		header_sent;
		bool		body_sent;
		bool		FileOpened;
		int			fd;

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

		std::string	getHttp_response( void );
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

		void		setFileIndex(size_t value);
		void		setFileEnd(size_t value);
		void		setWorker(Worker &worker);
		void		setHttp_request(request &request);
		void		setFileOpened( bool value );
		void		setFd( int value );

};
std::string			mime_types(std::string &extension);
#endif