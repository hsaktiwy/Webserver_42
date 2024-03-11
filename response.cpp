/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:15:52 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/03/08 16:51:06 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

const std::string convertToString(long long line)
{
    std::ostringstream convert;
    convert << line;

    return convert.str();
}


response::response(request &req, Worker &wk): http_request(&req), worker(&wk), body_index(0), body_size(-1), header_index(0), header_size(-1), body_sent(0), header_sent(0), FileOpened(false), fd(-1) ,FileSeeked(false) , Seeker(0), readyToResponed(false), StoringFile(false)
{

}

std::string 	ConnectionType(request &req)
{
	std::string buff;
	int result = req.getHeaderValue("Connection", buff);
	if (result == 1)
	{
		if (buff.find("keep-alive") != std::string::npos)
			return("keep-alive");
		else if (buff.find("close") != std::string::npos)
			return("close");
	}
	return ("keep-alive");
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

void    count_files(int *number_dir,int *number_file,const std::string & root)
{
    DIR *dir = opendir(root.c_str());
    struct  dirent *dirent;

    if (dir)
    {
        dirent = readdir(dir);
        dirent = readdir(dir);
        while ((dirent = readdir(dir)) != NULL)
        {
            std::string full_path = root + "/" + dirent->d_name;
            DIR *subdir = opendir(full_path.c_str());
            if (subdir)
            {
                (*number_dir)++;
                closedir(subdir);
            }
            else
                (*number_file)++;
			// closedir(subdir);
        }
        closedir(dir);
    }
}

void autoIndexing(request &req, Worker &wk, std::string &response_head, std::string &body, std::map<unsigned int, std::string> &status_codes)
{
    Uri const &uri = req.getUri();
    std::string path = uri.authority + "/" + uri.path;
    std::string File_name, Last_modification, Size, Link, Path;
    Path = wk.getRoot() + req.getUri().path;
    DIR *dir = opendir(Path.c_str());
    struct dirent *dirent;
    int number_dir = 0;
    int number_file = 0;

    count_files(&number_dir, &number_file, Path);
    response_head = "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=UTF-8\r\n";
    body = "<html><head><style>"
           "body { font-family: Arial, sans-serif; margin: 0; padding: 0; }"
           "h1 { padding: 20px; }"
           "table { width: 100%; border-collapse: collapse; }"
           "th, td { padding: 10px; border-bottom: 1px solid #ddd; text-align: left; }"
           "th { background-color: #f2f2f2; }"
           "tr:hover { background-color: #f5f5f5; }"
           "a { text-decoration: none; color: blue; }"
           ".folder { color: #007bff; }"
           ".file { color: #28a745; }"
           "</style></head>"
           "<body>"
           "<h1>Directory listing of " + wk.getRoot() + "</h1>\n"
           "<p style=\"margin-left:18px;\">directories <span class=\"folder\">" + convertToString(number_dir) + "</span> files <span class=\"file\">" + convertToString(number_file) + "</span></p><hr style=\"margin-bottom:30px;\">\n"
           "<table>\n<thead>\n<tr>\n<th style=\"width: 40%;\">File Name</th>\n<th style=\"width: 30%;\">Last modification</th>\n<th style=\"width: 30%;\">Size</th>\n</tr>\n</thead>\n<tbody>";

    if (dir)
    {
        while ((dirent = readdir(dir)) != NULL)
        {
            if (std::strcmp(dirent->d_name, ".") != 0)
            {
                Link = "/" + req.getUri().path + "/" + dirent->d_name;

                File_name = dirent->d_name, Size = "-", Last_modification = "-";
                Link = NormilisePath(Link);
                std::string tmpPath = Path + "/" + dirent->d_name;
                struct stat File_state;
                if (stat(tmpPath.c_str(), &File_state) == 0)
                {
                    std::stringstream ss;
                    ss << File_state.st_size;
                    ss >> Size;
                    Last_modification = TimeToString(File_state.st_mtimespec);
                }
                std::string line = "<tr>\n<td><a href=\"" + Link + "\">" + File_name + "</a></td>\n<td>" + Last_modification + "</td>\n<td>" + Size + "</td>\n</tr>";
                body += line;
            }
        }
        closedir(dir);
    }
    body += "</tbody>\n</table></body></html>";
    std::stringstream ss;
    std::string body_size;
    ss << body.size();
    ss >> body_size;
    response_head += "Content-Length: " + body_size + "\r\n\r\n";
	// printf("\n");
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
		readyToResponed = true;
		return ;
	}
	// printf("ola %d, %p %s %s\n", req.getError(), &req, req.getMethod().c_str(), req.getMethod_uri().c_str());
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
				readyToResponed = true;
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
		// printf("solo\n");
		errorresponse(status_codes);
		readyToResponed = true;
	}
	else
	{
		if (req.getMethod() == "GET")
		{
			Get(status_codes);
			readyToResponed = true;
		}
		else if (req.getMethod() == "POST")
		{
			Post(status_codes);
		}
		else
		{
			// this must be deleted
			body_string = "<html>\r\n<head>\r\n	<title>Valide File</title>\r\n</head>\r\n<body>\r\n	<h1>The response must be here!.</h1>\r\n</body>\r\n</html>\r\n";
			body_size = body_string.size();
			std::string Hconnection = "Connection: " + ConnectionType(req);
			http_response = "HTTP/1.1 200 OK\r\n" + Hconnection + "\r\nContent-Type: text/html\r\nServer: " + ((std::string)SERVERNAME) + "\r\nContent-Length: " + ToString(body_size) + "\r\n\r\n";
			header_size = http_response.size();
			readyToResponed = true;
		}
	}
}

// std::string	getFilename(std::stringstream &stream, std::string &boundary, bool &stop)
// {
// 	std::string filename;
// 	std::string buff;
// 	std::string boundary2 = "--" + boundary, boundaryEnd = "--" + boundary + "--";
// 	while (true)
// 	{
// 		if (getline(stream, buff, '\n'))
// 		{
// 			if (buff.find(boundary2) == std::string::npos && buff.find(boundaryEnd) == std::string::npos)
// 			{
// 				size_t	npos = buff.find("Content-Disposition");
// 				if (npos != std::string::npos && npos == 0)
// 				{
// 					std::string Format = "filename=";
// 					size_t npos2 = buff.find(Format);
// 					if (npos2 != std::string::npos)
// 					{
// 						npos2 += Format.size();
// 						ExtractValues(buff, filename, npos2);
// 						// adjust the stream this will surpace any line untell it get a empty line or in our case a line with only \r
// 						while (getline(stream, buff, '\n'))
// 						{
// 							if (buff == "\r")
// 								break;
// 						}
// 						return (filename);
// 					}
// 				}
// 			}
// 		}
// 		else
// 		{
// 			stop = true;
// 			break;
// 		}
// 	}
// 	return ("");
// }


// bool	OpenFile(std::string &filename, std::stringstream& stream, std::string &boundary, bool &stop)
// {
// 	std::ofstream	Outfile(filename.c_str());
// 	bool			run = true;
// 	std::string			buff;
// 	std::string boundary2 = "--" + boundary, boundaryEnd = "--" + boundary + "--";

// 	if (Outfile.is_open())
// 	{
// 		while (run)
// 		{
// 			if (getline(stream, buff, '\n'))
// 			{
// 				if (buff.find(boundary2) == std::string::npos && buff.find(boundaryEnd) == std::string::npos)
// 				{
// 					buff += '\n';
// 					Outfile << buff;
// 				}
// 				else
// 					break;
// 			}
// 			else
// 				stop = true, run = false;
// 		}
// 		Outfile.close();
// 		return (true);
// 	}
// 	else
// 		return (false);
// }

bool ft_getline(std::string &stream, size_t &index, std::string &buff,char delimiter)
{
	bool	isthere_data = (index < stream.size()) ? true : false;
	buff = "";
	while (index < stream.size())
	{
		
		if (stream[index] == delimiter)
		{
			index++;
			break;
		}
		buff += stream[index];
		index++;
	}
	return (isthere_data);
}

std::string	getFilenameExp(std::string &stream, size_t &index, std::string &boundary, bool &stop)
{
	std::string filename;
	std::string buff;
	std::string boundary2 = "--" + boundary, boundaryEnd = "--" + boundary + "--";
	while (true)
	{
		if (ft_getline(stream, index, buff, '\n'))
		{
			// printf("index %lu, buff : %s\n", buff.size(), buff.c_str());
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
						// printf("index_0_ %lu\n", index);
						int i = 0;
						while (ft_getline(stream, index, buff, '\n'))
						{
							// printf("buff  : %s\n", buff.c_str());
							if (buff == "\r")
								break;
							i++;
							if (i == 2)
								exit(0);
						}
						// printf("index_1_ %lu\n", index);
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

// this function will try to fill our file with the data from the body untell
// it reach the end (boundary) and return (1) mean it finished or it return 0 meaning it
// just get to the MaxWriteSize defined, else -1 for any error case
int	fillFile(int fd, std::string &stream, size_t &index, std::string &boundary, size_t MaxWriteSize)
{
	size_t		writtenBytes = 0;
	size_t		indexBegin = index;
	bool		finished = false;
	std::string boundary2 = "--" + boundary, boundaryEnd = "--" + boundary + "--";
	char *buff = (char *)malloc(sizeof(char) * MaxWriteSize);
	if (!buff)
		return (-1);
	while (index < stream.size() && writtenBytes < MaxWriteSize)
	{
		if (stream[index] == '\r' && index + 1 < stream.size() && stream[index + 1] == '\n' && index + 2 < stream.size() && stream[index + 2] == '-')
		{
			size_t j = 0;
			while (index + 2 + j < stream.size() && j < boundary2.size() && stream[index + 2 + j] == boundary2[j])
				j++;
			if (j == boundary2.size())
			{
				while (index + 2 + j < stream.size() && j < boundaryEnd.size() && stream[index + 2 + j] == boundaryEnd[j])
					j++;
				index += j + 2, finished = true;
				break;
			}
		}
		buff[writtenBytes] = stream[index];
		writtenBytes++;
		index++;
	}
	printf("writeBytes %lu, index %lu\n", writtenBytes, index);
	if (write(fd, buff, writtenBytes) == -1)
		return (free(buff), -1);
	return (free(buff), finished);
}

void	Post_no_body(request &req, std::string &http_response, long long &header_size, long long &body_size)
{
	std::string Hconnection = "Connection: " + ConnectionType(req);
	http_response = "HTTP/1.1 200 OK\r\n" + Hconnection + "\r\nContent-Type: text/html\r\nServer: " + ((std::string)SERVERNAME) + "Content-Length: " + ToString(body_size) + "\r\n\r\n";
	header_size = http_response.size();
	body_size = 0;
}

bool	response::PostInit(std::map<unsigned int, std::string> &status_codes, request &req, std::string &body)
{
	int		MFD_index = req.getHeaderIndex("Content-Type");
	if (MFD_index != -1)
	{
		std::vector<HTTPHeader> const & headers = req.getHeaders();
		HTTPHeader &header = (HTTPHeader&)headers[MFD_index];
		size_t valide = header.values.find("multipart/form-data");
		if (valide != std::string::npos)
		{
			body_index = 0;
			boundary = header.boundry;
			std::string buff;
			if (boundary.size() > 0)
				ft_getline(body, body_index, buff,'\n');
			POST_Init = true;
		}
		else
		{
			// Not impemented
			req.setStatus(501); req.setError(true);
			errorresponse(status_codes);
			readyToResponed = true;
			return (false);
		}
	}
	if (body.size() == 0)
	{
		Post_no_body(req, http_response, header_size, body_size);
		readyToResponed = true;
	}
	if (MFD_index == -1)
	{
		// printf("----->here\n");
		req.setStatus(400); req.setError(true);
		errorresponse(status_codes);
		readyToResponed = true;
		return (false);
	}
	return (true);
}

void	response::PostResponse(std::map<unsigned int, std::string> &status_codes)
{	
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
}

bool	response::PostFilesOpen(std::map<unsigned int, std::string> &status_codes, request &req, std::string &UploadPath)
{
	std::string path = UploadPath + "/" + CurrentFilename;
	printf("Path %s\n", path.c_str());
	fd = open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("open ");
		// printf("ola2\n");
		req.setStatus(500); req.setError(true);
		errorresponse(status_codes);
		readyToResponed = true;
		return (false);
	}
	if (fd > 0)
		files.push_back(CurrentFilename);
	return (true);
}

bool	response::PostFileFilling(std::map<unsigned int, std::string> &status_codes, request &req, std::string &body)
{
	int result = fillFile(fd, body, body_index, boundary, UPLOADCHUNK_SIZE);
	if (result == -1)
	{
		// handle error 500 in the server;
		req.setStatus(500); req.setError(true);
		errorresponse(status_codes);
		readyToResponed = true;
		return (false);
	}
	if (result == 1)
	{
		if (close(fd) == - 1)
		{
			req.setStatus(500); req.setError(true);
			errorresponse(status_codes);
			readyToResponed = true;
			return (false);
		}
		fd = -1;
		StoringFile = false;
	}
	return (true);
}

void	response::Post(std::map<unsigned int, std::string> &status_codes)
{
	request	&req = *http_request;
	Worker	&wk = *worker;
	std::string &body = (std::string &)req.getBody();
	std::string &UploadPath = wk.getPathUpload();
	bool	stop = false;

	// printf("Post handling\n");
	if (POST_Init == false)
	{
		// check first for the Content-Type  == multipart/form-data then define the boundary value
		if (!PostInit(status_codes, req, body))
			return ;
	}
	if (POST_Init == true)
	{
		// loop on all data relate to the files we want to create
		// eliminate boundary if there is one
		// get the filename from the Content-Disposition
		if (StoringFile == false)
			CurrentFilename = getFilenameExp(body, body_index, boundary, stop), StoringFile = true;
		// printf("filename %s, body_index %lu\n", CurrentFilename.c_str(), body_index);
		// first check for the file info and get all it data 
		if (stop == false && StoringFile == true)
		{
			if (!CurrentFilename.empty())
			{
				// open the file and start writing in the file
				if (fd == -1)
				{
					if (!PostFilesOpen(status_codes, req, UploadPath))
						return ;
				}
				if (fd > 0)
				{
					// if (!OpenFile(path, stream, boundary, stop))
					if (!PostFileFilling(status_codes, req, body))
						return ;
					// printf("index_body %lu, stop %d result %d\n", body_index, stop);
				}
			}
		}
		if (stop == true)
		{
			body_index = 0;
			readyToResponed = true;
		}
		// create the response
	}
	if (readyToResponed)
		PostResponse(status_codes);
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
	std::string Hconnection = "Connection: " + ConnectionType(req);
	http_response = "HTTP/1.1 "+ Code +"\r\n" + Hconnection + "\r\nContent-Type: " + FileType + "\r\n";
	file = wk.getRoot() + "/" + req.getUri().path;
	header_size = http_response.size();
}


void    response::RedirectionResponse(std::map<unsigned int, std::string> &status_codes, std::string &path)
{
	Worker &wk = *worker;
	request &req = *http_request;
	std::string HumanRead;
	std::stringstream ss;
	std::string statusCode;

	req.setStatus(302);
	std::map<unsigned int, std::string>::iterator iter = status_codes.find(req.getStatus());
	ss << req.getStatus();
	ss >> statusCode;
	if (iter != status_codes.end())
		HumanRead = iter->second;
	std::string Hconnection = "Connection: " + ConnectionType(req);
	http_response = "HTTP/1.1 " + statusCode + " " + HumanRead + "\r\n" + Hconnection + "\r\nContent-Type: text/html\r\n" + "Location: "+ path + "\r\nServer: " + ((std::string)SERVERNAME) + "\r\n\r\n";
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
	// printf("::::::::::::%d __ %s \n", wk.get_track_status(), wk.getPathError().c_str());
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
		http_response += "Connection: close\r\nContent-Type: text/html\r\n";
		size_t size = body_string.size();
		http_response += "Content-Length: " + ToString(size) + "\r\nServer: " + ((std::string)SERVERNAME) + "\r\n\r\n";
		body_size = 0;
		http_response += body_string;
		header_size = http_response.size();
		// printf("Reponse : %s\n", http_response.c_str());
		// printf("%s", body_string.c_str());
	}
	else
	{
		// printf("error %d\n", req.getError());
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
		// printf("Copy Assigned oPERATOR IS CALLED\n");
		// printf("Before setting ->%lld %lu\n", header_size, header_index);
		// printf("To Copy setting %lld %lu\n", obj.header_size, obj.header_index);
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
		FileSeeked = obj.FileSeeked;
		Seeker = obj.Seeker;
		readyToResponed = obj.readyToResponed;
		StoringFile = obj.StoringFile;
		POST_Init = obj.POST_Init;
		// stream = obj.stream;
		index = obj.index;
		boundary = obj.boundary;
		CurrentFilename = obj.CurrentFilename;
		files = obj.files;
		// printf("After setting %lld %lu\n", header_size, header_index);
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

// GETTERS

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

bool	response::getFileSeeked( void ) const
{
	return (FileSeeked);
}
size_t	response::getSeeker( void ) const
{
	return (Seeker);
}


// SETTERS
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


bool	response::getReadyToResponed( void ) const
{
	return this->readyToResponed;
}


void        response::setFileIndex(size_t value)
{
	FileIndex = value;
}

void        response::setFileEnd(size_t value)
{
	FileEnd = value;
}

void	response::setFileSeeked(bool value)
{
	FileSeeked = value;
}
void	response::setSeeker(size_t value)
{
	Seeker = value;
}

void        response::setHttp_request(request &request)
{
	http_request = &request;
}

void        response::setWorker(Worker &wk)
{
	worker = &wk;
}
