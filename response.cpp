/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:15:52 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/02/10 18:38:44 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

response::response(request &req, Worker &wk): http_request(&req), worker(&wk), body_index(0), body_size(-1), header_index(0), header_size(-1), body_sent(0), header_sent(0), FileOpened(false), fd(-1)
{

}

std::string TimeToString(timespec time)
{
	struct tm *data = std::gmtime(&time.tv_sec);
	std::string Year,Month,Day;
	int year, mon,day;
	year = 1900 + data->tm_year;
	mon = data->tm_mon + 1;
	day = data->tm_mday;
	std::stringstream ss;
	ss << year;
	ss >> Year;
	ss.str("");
	ss.clear();
	ss << mon;
	ss >> Month;
	ss.str("");
	ss.clear();
	ss << day;
	ss >> Day;
	return (Year + "-" + Month + "-" + Day);
}

void    autoIndexing(request &req, Worker &wk,std::string &response_head, std::string &body, std::map<unsigned int, std::string> &status_codes)
{
	Uri const &uri = req.getUri();
	std::string path = uri.authority + "/" +uri.path;
	std::string File_name, Last_modification, Size, Link, Path;
	Path = wk.getRoot() + req.getUri().path;
	DIR     *dir = opendir(Path.c_str());
	struct  dirent *dirent;

	response_head = "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=UTF-8\r\nServer: " + ((std::string)SERVERNAME) + "\r\n";
	body = "<table>\n<thead>\n    <tr>\n        <th>File Name</th>\n        <th>Last modification</th>\n        <th>Size</th>\n    </tr>\n</thead>\n<tbody>";
	if (dir)
	{
		while ((dirent = readdir(dir)) != NULL)
		{
			if (std::strcmp(dirent->d_name, ".") != 0)
			{
				Link = "/" + req.getUri().path + "/" + dirent->d_name;
				
				File_name = dirent->d_name,Size = "-", Last_modification = "-";
				Link = NormilisePath(Link);
				std::string tmpPath =  Path + "/" + dirent->d_name;
				struct stat File_state;
				if (stat(tmpPath.c_str(), &File_state) == 0)
				{
					std::stringstream ss;
					ss << File_state.st_size;
					ss >> Size;
					Last_modification = TimeToString(File_state.st_mtimespec);
				}
				std::string line = "\n<tr>\n<td><a href=\"" +  Link +"\">"+ File_name +"</a></td>\n<td>" + Last_modification + " </td>\n<td>"+ Size +"</td>\n</tr>";
				body += line;
			}
		}
		closedir(dir);
	}
	body += "\n</tbody>\n</table>";
	// printf("======================>AutoIndexing\n");
	std::stringstream ss;
	std::string body_size;
	ss << body.size();
	ss >> body_size;
	response_head += "Content-Length: " + body_size + "\r\n\r\n";
}


std::string GetFileType(const std::string &Path)
{
	size_t pos =  Path.rfind('.');
	if (pos != std::string::npos)
	{
		std::string extension = &Path[pos + 1];
		return (mime_types(extension));
	}
	return ("text/plain");
}

void    response::responed(std::map<unsigned int, std::string> &status_codes)
{
	std::string index = (*worker).getIndex();
	request &req = *http_request;
	Worker &wk = *worker;

	// Redirection Case
	if (!wk.getRedirect().empty())
	{
		std::string path =  wk.getRedirect();
		RedirectionResponse(status_codes, path);
		return ;
	}

	if (req.getError() == false)
	{
		if (req.getIs_dir() == 1 || req.getIs_regular() == 1)
		{
			if (req.getMethod() == "GET" && req.getIs_dir() == 1 &&  wk.getAutoIndex() == "on")
			{
				// autoindexing
				autoIndexing(req, wk,http_response, body_string, status_codes);
				header_size = http_response.size();
				body_size = body_string.size();
				return ;
			}
			else if (req.getMethod() == "GET" && req.getIs_dir() == 1 && index.size() == 0)
			{
				req.setError(true), req.setStatus(404);
			}
		}
		else
		{
			req.setError(true), req.setStatus(404);
		}
	}
	// responed using error pages if  (error  == true)
	if (req.getError() == true)
	{
		// response for Error handling
		errorresponse(status_codes);
	}
	else
	{
		if (req.getMethod() == "GET")
		{
			Get(status_codes);
		}
		else if (req.getMethod() == "POST")
		{
			Post(status_codes);
		}
		else
		{
			body_string = "<html>\r\n<head>\r\n	<title>Valide File</title>\r\n</head>\r\n<body>\r\n	<h1>The response must be here!.</h1>\r\n</body>\r\n</html>\r\n";
			body_size = body_string.size();
			http_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nServer: " + ((std::string)SERVERNAME) + "Content-Length: " + ToString(body_size) + "\r\n\r\n";
			header_size = http_response.size();

		}
	}
}

std::string	getFilename(std::stringstream &stream, std::string &boundary, bool &stop)
{
	std::string filename;
	std::string buff;
	std::string boundary2 = "--" + boundary, boundaryEnd = "--" + boundary + "--";
	while (true)
	{
		if (getline(stream, buff, '\n'))
		{
			if (buff.find(boundary2) == std::string::npos && buff.find(boundaryEnd) == std::string::npos)
			{
				size_t	npos = buff.find("Content-Disposition");
				if (npos != std::string::npos && npos == 0)
				{
					std::string Format = "filename=";
					size_t npos2 = buff.find(Format);
					if (npos2 != std::string::npos)
					{
						npos2 += Format.size();
						ExtractValues(buff, filename, npos2);
						// adjust the stream this will surpace any line untell it get a empty line or in our case a line with only \r
						while (getline(stream, buff, '\n'))
						{
							if (buff == "\r")
								break;
						}
						return (filename);
					}
				}
			}
		}
		else
		{
			stop = true;
			break;
		}
	}
	return ("");
}

bool	OpenFile(std::string &filename, std::stringstream& stream, std::string &boundary, bool &stop)
{
	std::ofstream	Outfile(filename.c_str());
	bool			run = true;
	std::string			buff;
	std::string boundary2 = "--" + boundary, boundaryEnd = "--" + boundary + "--";

	if (Outfile.is_open())
	{
		while (run)
		{
			if (getline(stream, buff, '\n'))
			{
				if (buff.find(boundary2) == std::string::npos && buff.find(boundaryEnd) == std::string::npos)
				{
					buff += '\n';
					Outfile << buff;
				}
				else
					break;
			}
			else
				stop = true, run = false;
		}
		Outfile.close();
		return (true);
	}
	else
		return (false);
}

void	response::Post(std::map<unsigned int, std::string> &status_codes)
{
	std::string UploadPath = "Uploads/";
	request	&req = *http_request;
	Worker	&wk = *worker;
	int		MFD_index = req.getHeaderIndex("Content-Type");
	bool	stop = false;
	std::vector<std::string> files;

	// check first for the Content-Type  == multipart/form-data then define the boundary value
	if (MFD_index != -1)
	{
		printf("Solo leveling\n");
		std::vector<HTTPHeader> const & headers = req.getHeaders();
		HTTPHeader &header = (HTTPHeader&)headers[MFD_index];
		size_t valide = header.values.find("multipart/form-data"); 
		if (valide != std::string::npos)
		{
			std::string &boundary = header.boundry;
			std::string &body = (std::string &)req.getBody();
			std::stringstream stream(body);
			std::string buff;
			// loop on all data relate to the files we want to create
			// eliminate boundary if there is one
			if (boundary.size() > 0)
				getline(stream, buff, '\n');
			while (!stop)
			{
				// get the filename from the Content-Disposition
				std::string filename = getFilename(stream, boundary, stop);
				printf("filename %s\n", filename.c_str());
				// first check for the file info and get all it data 
				if (!filename.empty())
				{
					// open the file and start writing in the file
					std::string path = UploadPath + "/" + filename;
					printf("Path %s\n", path.c_str());
					if (!OpenFile(path, stream, boundary, stop))
					{
						// handle error 500 in the server;
						req.setStatus(500); req.setError(true);
						errorresponse(status_codes);
						return ;
					}
					else
						files.push_back(filename);
				}
			}
			// create the response
			int code;
			if (files.size() == 0)
				code = 200;
			else	
				code = 201;
			std::string human_read = Status(code, status_codes);
			http_response = "HTTP/1.1 " + human_read + "\r\nContent-Type: text/plain\r\n";
			for(size_t i = 0; i < files.size(); i++)
				body_string += "Resource is Created : " + files[i] + "\r\n"; 
			body_size = body_string.size();
			http_response += "Content-Length: " + ToString(body_size) + "\r\n\r\n";
			header_size = http_response.size();
			printf("%s\n%s\n", http_response.c_str(), body_string.c_str());
		}
		else
		{
			// Not impemented
			req.setStatus(501); req.setError(true);
			errorresponse(status_codes);
			return ;
		}
	}

	if (MFD_index == -1)
	{
		printf("Solo leveling2\n");
		// in case where we don't knew the type of data that we are handling
		// 400 bad request;
		req.setStatus(400); req.setError(true);
		errorresponse(status_codes);
		return ;
	}
	// where is should put the file  for new let put them in
	// std::vector<HTTPHeader> &header = req.getHeaders();
}

void	response::Get(std::map<unsigned int, std::string> &status_codes)
{
	request &req = *http_request;
	Worker &wk = *worker;

	FileType = GetFileType(req.getUri().path);
	int Range_index = http_request->getHeaderIndex("Range");
	std::string Code;
	if (Range_index == -1)
		Code =  Status(200, status_codes);
	else
		Code =  Status(206, status_codes);
	http_response = "HTTP/1.1 "+ Code +"\r\nContent-Type: " + FileType + "\r\n";
	file = wk.getRoot() + req.getUri().path;
	header_size = http_response.size();
}

void    response::RedirectionResponse(std::map<unsigned int, std::string> &status_codes, std::string &path)
{
	Worker &wk = *worker;
	request &req = *http_request;
	std::string HumanRead;
	std::stringstream ss;
	std::string statusCode;

	req.setStatus(301);
	std::map<unsigned int, std::string>::iterator iter = status_codes.find(req.getStatus());
	ss << req.getStatus();
	ss >> statusCode;
	if (iter != status_codes.end())
		HumanRead = iter->second;
	http_response = "HTTP/1.1 " + statusCode + " " + HumanRead + "\r\n" + "Content-Type: text/html\r\n" + "Location: "+ path + "\r\nServer: " + ((std::string)SERVERNAME) + "\r\n";
	header_size = http_response.size();
	body_size = 0;
}

void    response::errorresponse(std::map<unsigned int, std::string> &status_codes)
{
	Worker &wk = *worker;
	request &req = *http_request;
	std::string HumanRead;
	std::stringstream ss;
	std::string statusCode;

	worker->setPathError(worker->getErrorPages(), req.getStatus(),worker->getRoot());
	if (wk.get_track_status() == 0 || (wk.get_track_status() == 1 && wk.getPathError().empty()))
	{
		// printf("::::::::::::%d __ %s \n", wk.get_track_status(), wk.getPathError().c_str());
		if (wk.get_track_status() == 1 && wk.getPathError() == "")
			req.setStatus(404);
		std::map<unsigned int, std::string>::iterator iter = status_codes.find(req.getStatus());
		ss << req.getStatus();
		ss >> statusCode;
		if (iter != status_codes.end())
			HumanRead = iter->second;
		// this maybe need to be in a folder i mean like a error_page file but with out being define in the http configue file
		body_string = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Error Page</title>\r\n<style>\r\nbody {\r\nfont-family: Arial, sans-serif;\r\ntext-align: center;\r\npadding-top: 50px;\r\n}\r\nh1 {\r\nfont-size: 3em;\r\ncolor: #990000;\r\nmargin-bottom: 20px;\r\n}\r\np {\r\nfont-size: 1.5em;\r\ncolor: #666666;\r\nmargin-bottom: 50px;\r\n}\r\n</style>\r\n</head>\r\n<body>\r\n<h1>Error "+ statusCode + "("+ HumanRead +")"+"</h1>\r\n<p>Unhable to reserve a propore response.</p>\r\n</body>\r\n</html>";
		http_response += "HTTP/1.1 " + statusCode + " " + HumanRead + "\r\n";
		http_response += "Content-Type: text/html\r\n";
		std::string size;
		ss << body_string.size();
		ss >> size;
		http_response += "Content-Length: " + size + "\r\nServer: " + ((std::string)SERVERNAME) + "\r\n\r\n";
		body_size = body_string.size();
		header_size = http_response.size();
	}
	else
	{
		std::string path = wk.getLocationWorker().getPath() + "/" + worker->getPathError();
		path = NormilisePath(path);
		RedirectionResponse(status_codes, path);
	}
}

std::string response::getHttp_response( void )
{
	return (http_response);
}

response::response():http_request(NULL), worker(NULL), body_index(0), body_size(-1), header_index(0), header_size(-1), body_sent(0), header_sent(0), FileOpened(false), fd(-1)
{

}

response::~response()
{
}

response::response(const response& copy)
{
	*this = copy;
}

response& response::operator=(const response& obj)
{
	if (this != &obj)
	{
		printf("Copy Assigned oPERATOR IS CALLED\n");
		printf("Before setting ->%lld %lu\n", header_size, header_index);
		printf("To Copy setting %lld %lu\n", obj.header_size, obj.header_index);

		http_response = obj.http_response;
		http_request = obj.http_request;
		body_string = obj.body_string;
		file = obj.file;
		header_index = obj.header_index;
		body_index = obj.body_index;
		FileIndex = obj.FileIndex;
		FileEnd = obj.FileEnd;
		FileType = obj.FileType;
		header_size = obj.header_size;
		body_size = obj.body_size;
		header_sent = obj.header_sent;
		body_sent = obj.body_sent;
		FileOpened = obj.FileOpened;
		fd = obj.fd;
		printf("After setting %lld %lu\n", header_size, header_index);
	}
	return (*this);
}

std::string response::Status(unsigned int status, std::map<unsigned int, std::string> &status_codes)
{
	std::string result;
	std::map<unsigned int, std::string>::iterator iter = status_codes.find(status);
	if (iter != status_codes.end())
	{
		std::stringstream   ss;
		ss << status;
		ss >> result;
		result += " " +iter->second;
	} 
	return (result);
}

std::string response::getHttp_response( void ) const
{
	return (http_response);
}

std::string response::getBody_string( void ) const
{
	return (body_string);
}

std::string response::getFile( void ) const
{
	return (file);
}


void        response::setHttp_request(request &request)
{
	http_request = &request;
}

void        response::setWorker(Worker &wk)
{
	worker = &wk;
}

size_t      response::getHeader_index( void ) const
{
		return (header_index);
}

long long   response::getHeader_size( void ) const
{
	return (header_size);
}

long long   response::getBody_size( void ) const
{
	return (body_size);
}

size_t      response::getBody_index( void ) const
{
	return (body_index);
}

bool        response::getHeader_sent( void ) const
{
	return (header_sent);
}

bool        response::getBody_sent( void ) const
{
	return (body_sent);
}

bool    response::getFileOpened( void ) const
{
	return (FileOpened);
}

int     response::getFd( void ) const
{
	return (fd);
}

std::string response::getFileType( void ) const
{
	return (FileType);
}

void        response::setHeader_index(size_t value)
{
	header_index = value;
}

void        response::setHeader_size(long long  value)
{
	header_size = value;
}

void        response::setBody_size(long long  value)
{
	body_size = value;
}

void        response::setBody_index(size_t  value)
{
	body_index = value;
}

void        response::setHeader_sent(bool  value)
{
	header_sent = value;
}

void        response::setBody_sent( bool  value)
{
	body_sent = value;
}


void    response::setFileOpened( bool value )
{
	FileOpened = value;
}

void     response::setFd( int value )
{
	fd = value;
}

size_t      response::getFileIndex( void ) const
{
	return (FileIndex);
}

size_t      response::getFileEnd( void ) const
{
	return (FileEnd);
}

void        response::setFileIndex(size_t value)
{
	FileIndex = value;
}

void        response::setFileEnd(size_t value)
{
	FileEnd = value;
}
