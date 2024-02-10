/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaktiwy <hsaktiwy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:15:46 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/02/10 14:52:34 by hsaktiwy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"
#include "./cgi/cgi.hpp"

request::request(): RequestRead(false), Parsed_StartLine(false), Parsed_Header(false), Body_Exist(false), Parsed_Body(false), ContentLengthExist(false), HandleRequest(false), R_Method(false), R_URI(false), R_PROTOCOL(false), R_FUll_HEADERS(false), R_FULL_BODY(false)
{
	BodyLimiterType = 0;
	FillingBuffer = false;
	SLValidity = false;
	request_length = 0;
	error = false;
	status = 200;
	left_CR = false;
	NewLine = false;
	R_HEADER = true;
	R_VALUE = true;
	BIndex = 0;
	ChunkedRead = false;
	ChunkedSizeRead = false;
	ChunkedSize = 0;
}

static bool CharacterUri(char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ':'
		|| (c >= '0' && c <= '9') ||   c == '/' || c == ';' || c == '?'
		|| c == '#' || c == '$' || c == '-' || c == '_' || c == '.' 
		|| c == '+' || c == '!' || c == '*' || c == '\'' || c == '('
		|| c == ')' || c == ',' || c == '~' || c == '@' || c == '&'
		|| c == '=' || c == '%' ||  c == '[' || c == ']' )
		return (true);
	return (false);
}

static bool CheckUriFormat(std::string &uri)
{
	for(int i = 0; uri[i]; i++)
	{
		if (!CharacterUri(uri[i]))
			return (false);
	}
	return (true);
}


static	bool	getToken(char *buff, size_t &index, ssize_t bytes, std::string &holder, bool &boolean, bool &readStatus)
{
	while (index < bytes && buff[index] != ' ')
	{
		if (buff[index] == '\r' || buff[index] == '\n')
			return (false);
		holder += buff[index++];
		readStatus = true;
	}
	// if i reach buffer limites
	if (index < bytes && buff[index] == ' ')
	{
		readStatus = false;
		boolean = true;	
	}
	return (true);
}

bool	is_ctl(char c)
{
	if ((c >= 0 && c <= 31) || c == 127)
		return (true);
	return (false);
}

bool	is_seperator(char c)
{
	if (c == '(' || c == ')' || c == '<' || c == '>' || c == '@'
		|| c == ',' || c == ';' || c == ':' || c == '\\' || c == '\"'
		|| c == '/' || c == '[' || c == ']' || c == '?' || c == '='
		|| c == '{' || c == '}' || c == SP || c == HT )
		return (true);
	return (false);
}

bool	ishexa(char c)
{
	if (std::isdigit(c) || c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F'
    || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f')
		return (true);
	return (false);
}

size_t	hexaToDecimal(std::string &str)
{
	std::stringstream ss(str);
    size_t s;
    ss >> std::hex >> s;
    return (s);
}

void	AddEmptyNode(std::vector<HTTPHeader> &vec)
{
	HTTPHeader empty;
	vec.push_back(empty);
}

void	request::ParseRequest(char *buff, ssize_t bytes_size)
{
	std::string allowedMethod[] = {"POST", "GET", "DELETE"};
	size_t index = 0;

	if (!Parsed_StartLine)
	{
		
		if (!R_Method)
		{

			// check if the buff start with sapce this case is not valide at all
			if (FillingBuffer == false && index < bytes_size && buff[index] == ' ')
			{
				error = true, RequestRead = true, status = 400;
				return ;
			}
			// new let us get our method if it possible
			if (!getToken(buff, index, bytes_size, method, R_Method, FillingBuffer))
			{
				error = true, RequestRead = true, status = 400;
				return ;
			}
		}
		if (R_Method && !R_URI && index < bytes_size)
		{
			// i will try to avoid all spaces
			while (FillingBuffer == false && index < bytes_size && buff[index] == ' ')
				index++;
			// now let try to get the Fully URI
			if (!getToken(buff, index, bytes_size, method_uri, R_URI, FillingBuffer))
			{
				error = true, RequestRead = true, status = 400;
				return ;
			}
		}
		if (R_URI && !R_PROTOCOL && index < bytes_size)
		{
			while (FillingBuffer == false && index < bytes_size && buff[index] == ' ')
				index++;
			// now let try to get the Fully URI
			while (index < bytes_size && buff[index] != ' ')
			{
				// suppose we will handle only the \r\n clients request
				if (buff[index] == '\r' || buff[index] == '\n')
					break;
				http += buff[index++];
				FillingBuffer = true;
			}
			//imagine someone sente a request with a space after the protocol 
			if (index < bytes_size && buff[index] != '\r')
			{
				error = true, RequestRead = true, status = 400;
				return ;
			}
			else if (FillingBuffer && index < bytes_size && buff[index] == '\r')
				FillingBuffer = false, R_PROTOCOL = true;
			if (index < bytes_size && buff[index] == '\r' &&  index + 1 < bytes_size && buff[index + 1] == '\n')
				index +=2,Parsed_StartLine = true, NewLine = true;
			else if (index < bytes_size && buff[index] == '\r' &&  index + 1 == bytes_size)
			{
				index++, left_CR = true;
				return ;
			}
			else if (index < bytes_size && buff[index] == '\r' && index + 1 < bytes_size && buff[index + 1] != '\n')
			{
				error = true, RequestRead = true, status = 400;
				return ;
			}
		}
		if (!SLValidity && R_Method && R_URI && R_PROTOCOL)
		{
			bool MethodeExist = false;
			for (int i = 0; i < 3; i++)
			{
				if (method == allowedMethod[i])
				{
					MethodeExist = true;
					if (allowedMethod[i] == "POST")
						Body_Exist = true;
					break;
				}
			}
			if (http != "HTTP/1.1" || !MethodeExist || !CheckUriFormat(method_uri))
			{
				error = true, RequestRead = true, status = 400;
				return ;
			}
			SLValidity = true;
		}
		if (left_CR == true && index < bytes_size && buff[index] == '\n')
			Parsed_StartLine = true, left_CR = false, index++,NewLine = true;
		else if (left_CR == true && index < bytes_size && buff[index] != '\n')
		{
			error = true, RequestRead = true, status = 400;
			return ;
		}
	}
	// check if the header is parsed
	if (Parsed_StartLine && !Parsed_Header && index < bytes_size)
	{
		if (!R_FUll_HEADERS)
		{
			while (index < bytes_size)
			{
				// when i should create a header ? when i have flag saying that i passed new line
				// when i need to stop this shite when i get new line the annalyse a new line after
				if ((left_CR == false && buff[index] == '\r' && index + 1 < bytes_size && buff[index + 1] == '\n' && NewLine)/*In normale case where \r\n are are not splited*/
				|| (buff[index] == '\n' && left_CR && NewLine))
				{
					NewLine = false, R_VALUE = false, R_HEADER = false;
					index +=((left_CR) ? 1:2), R_FUll_HEADERS = true;
					left_CR = false;
					break;
				}
				if (left_CR && buff[index] == '\n' && !NewLine)
				{
					index += 1;
					if (FillingBuffer && !R_HEADER)
					{
						error = true, RequestRead = true, status = 400;
						return ;
					}
					else if (R_HEADER)
					{
						NewLine = true, R_VALUE = true, R_HEADER = true;
						left_CR = false;
						FillingBuffer = false;
						if (index == bytes_size)
							break;
					}
					left_CR = false;
					NewLine = true;
				}
				if (buff[index] == '\r' && index + 1 == bytes_size)
				{
					index +=1, left_CR = true;
					break;
				}
				if (NewLine)
				{
					NewLine = false, R_VALUE = false, R_HEADER = false;
					FillingBuffer = false;
					if (index < bytes_size)
						AddEmptyNode(headers);
					continue;
				}
				if (!R_HEADER && !is_seperator(buff[index]) && !is_ctl(buff[index]))
				{
					headers[headers.size() - 1].name += buff[index];
					FillingBuffer = true;
				}
				else if (!R_HEADER && buff[index] == ':')
				{
					R_HEADER = true, index++, FillingBuffer = false;
					continue;
				}
				else if (!R_HEADER && (is_seperator(buff[index]) || is_ctl(buff[index])))
				{
					error = true, RequestRead = true, status = 400;
					return ;
				}
				if (R_HEADER && !R_VALUE && buff[index] != '\r')
				{
					headers[headers.size() - 1].values += buff[index];
					FillingBuffer = true;
				}
				if (R_HEADER && !R_VALUE && buff[index] == '\r' && index + 1 < bytes_size && buff[index + 1] == '\n')
				{
					index += 2, NewLine = true, R_VALUE = true;
					FillingBuffer = false;
					continue;
				}
				if (R_HEADER && !R_VALUE && buff[index] == '\r' && index + 1 < bytes_size && buff[index + 1] != '\n')
				{
					error = true, RequestRead = true, status = 400;
					return ;
				}
				index++;
			}
			// when i need stop the header insertion, when i passed new line
		}
		if (R_FUll_HEADERS)
		{
			// parse the headers
			printf("method %s && uri %s && %s\n", method.c_str(), method_uri.c_str(), http.c_str());
			for(int i = 0; i < headers.size(); i++)
			{
				printf("%s : %s\n", headers[i].name.c_str(), headers[i].values.c_str());
			}

			// define the body limiter 
			// call them in this orther will made sure the priority so be carefull
			// check if we have contnetlength header
			for(size_t i = 0; i < headers.size(); i++)
			{
				headers[i].values = ft_trim(headers[i].values, " ");
			}
			int HOSTindex = getHeaderIndex("Host");
			if (HOSTindex != -1)
			{
				int i = 0;
				host = &headers[HOSTindex].values[i];
			}
			else
			{
				error = true, RequestRead = true, status = 400;
				return ;
			}
			
			int CLIndex = getHeaderIndex("Content-Length");
			if (CLIndex != -1)
			{
				ContentLengthExist =  true;
				ContentLengthSize = ft_atoll(headers[CLIndex].values.c_str());
				BodyLimiterType = 1;
			}
			// even thougth there is a content length check for the boundary is if there
			int CTindex = getHeaderIndex("Content-Type");
			if (CTindex != -1)
			{
				std::string Format = "multipart/form-data;";
				// check for the multipart/form-data
				size_t pos = headers[CTindex].values.find(Format);
				if (pos != std::string::npos)
				{
					std::string express = "boundary=";
					size_t pos2 = headers[CTindex].values.find(express);
					if (pos2 != std::string::npos && pos2 > pos + Format.size())
					{
						pos2 += express.size();
						ExtractValues(headers[CTindex].values, boundary, pos2);
						headers[CTindex].boundry = boundary;
						boundary = "--" + boundary + "--";
						BodyLimiterType = 3;
					}
				}
			}
			// check transfer-encoding
			int TEindex = getHeaderIndex("Transfer-Encoding");
			if (TEindex != -1)
			{
				std::string Format = "chunked";
				// check for the chunked
				size_t pos = headers[TEindex].values.find(Format);
				if (pos != std::string::npos)
					BodyLimiterType = 2;
			}
			Parsed_Header = true;
			FillingBuffer = false;
			left_CR = false;
			printf("Does the bodyExist %d, and what delimiter whe need %d\n", Body_Exist, BodyLimiterType);
			// exit(0);
		}
	}
	// check the body existance the read and define the end of it
	if (Parsed_StartLine && Parsed_Header && Body_Exist)
	{
		if (BodyLimiterType == 0)
		{
			RequestRead = true;
		}
		else if (BodyLimiterType == 1)
		{
			while (index < bytes_size)
			{
				body += buff[index];
				if (buff[index] != '\r' || buff[index] != '\n')
					BIndex++;
				index++;
			}
			if (BIndex == ContentLengthSize)
				RequestRead = true;
			else if (BIndex > ContentLengthSize)
			{
				error = true, RequestRead = true, status = 400;
				return ;
			}
		}
		else if (BodyLimiterType == 2)
		{
			// first i need to define the size of the chunked
			while (index < bytes_size)
			{
				if (!ChunkedSizeRead && ishexa(buff[index]))
				{
					FillingBuffer = true;
					ChunkSizeString += buff[index];
				}
				if ((!ChunkedSizeRead && buff[index] == '\r' && index + 1 < bytes_size && buff[index + 1] == '\n')
				||	(!ChunkedSizeRead && left_CR && buff[index] == '\n'))
				{
					ChunkedSizeRead = true;
					FillingBuffer = false;
					index +=(left_CR) ? 1:2;
					left_CR = false;
					ChunkedSize = hexaToDecimal(ChunkSizeString);
					ChunkSizeString = "";
					BIndex = 0;
					if (ChunkedSize == 0)
					{
						RequestRead = true;
						return ;
					}
					continue;
				}
				if (ChunkedSizeRead && BIndex < ChunkedSize)
				{
					FillingBuffer = true;
					body += buff[index];
					BIndex++;
				}
				if (ChunkedSizeRead && FillingBuffer && BIndex == ChunkedSize)
				{
					if ((buff[index] == '\r' && index + 1 < bytes_size && buff[index + 1] == '\n')
						||	(left_CR && buff[index] == '\n'))
					{
						index +=(left_CR) ? 1 : 2, ChunkedSizeRead = false, FillingBuffer = false, left_CR = false;
						continue;
					}
					if (buff[index] == '\r' && index + 1 == bytes_size)
						index++, left_CR = true;
				}
				if (!ChunkedSizeRead && buff[index] == '\r' && index + 1 == bytes_size)
				{
					index++, left_CR = true;
					break;
				}
				index++;
			}
		}
		else if (BodyLimiterType == 3)
		{
			static std::string tmp;
			while (index < bytes_size)
			{
				body += buff[index];
				if (buff[index] == '-' && !FillingBuffer)
				{
					tmp +=buff[index];
					FillingBuffer = true;
				}
				else if (FillingBuffer && buff[index] != '\r')
				{
					tmp += buff[index];
				}
				else if (FillingBuffer && buff[index] == '\r')
				{
					if (tmp == boundary)
					{
						RequestRead = true;
						return ;
					}
					tmp = "";
					FillingBuffer = false;
				}
				index++;
			}
			if (tmp == boundary)
			{
				RequestRead = true;
				return ;
			}
		}
	}
	else if (Parsed_StartLine && Parsed_Header)
	{
		RequestRead = true;
	}
}

static bool absoluteURI(std::string &uri)
{
	if (uri.size() > 4)
	{
		bool http = (uri.find("http:://") == 0) ? true : false, https = (uri.find("http:://") == 0) ? true : false;
		if (http || https)
			return (true);
	}
	return (false);
}

static void	FillUriStructor(t_uri& uri, std::string &full_uri, bool authority)// authority boolean will confirme if there is a authority in the uri or note
{
	size_t size = 0;
	size += uri.scheme.size(); 
	// extract the authority
	if (authority)
	{
		for (size_t i = size; full_uri[i] && full_uri[i] != '/' && full_uri[i] != '?' && full_uri[i] != '#'; i++)
			uri.authority += full_uri[i];
		size += uri.authority.size();
	}
	if (full_uri[size] && full_uri[size] == '/')
		size++;
	// extract the path
	for (size_t i = size; full_uri[i] && full_uri[i] != '?' && full_uri[i] != '#'; i++)
		uri.path += full_uri[i];
	size += uri.path.size();
	if (full_uri[size] && full_uri[size] == '?')
		size++;
	// extract the query
	for(size_t i = size; full_uri[i] && full_uri[i] != '#'; i++)
		uri.query += full_uri[i];
	size += uri.query.size();
	if (full_uri[size] && full_uri[size] == '#')
		size++;
	// extract the fragment  i think we will remove this line and it logic in the future
	for (size_t i = size; full_uri[i]; i++)
		uri.fragment += full_uri[i];
}

static void UriFormat(t_uri &uri, std::string &full_uri, std::string &host)// 1 for absolute 2 for relative 3 for autority 
{
	// check it the uri start with http or https
	//[scheme]://[authority]/[path]?[query]#[fragment]
	bool	absoluteUri = absoluteURI(full_uri);
	FillUriStructor(uri, full_uri, false), uri.type = RELATIVE;
}

bool	ft_strcmp(const char *s1, const char *s2)
{
	for(int i = 0; s1[i] || s2[i]; i++)
	{
		if (s1[i] != s2[i])
			return (false);
	}
	return (true);
}

void	request::CheckRequest(std::vector<ServerBlocks> &serverBlocks, Worker& worker)
{
	is_dir = 0;
	is_regular = 0;
	std::string KnownHeaders[] = {"Host", "Accept", "Accept-Language", "Accept-Encoding", "Connection", "Referer"};
	std::string mimeType[] = {"image/avif", "image/avif", "image/jpeg", "image/gif", "image/png", "text/csv",  "text/html",   "text/javascript", "text/plain", "text/xml", "text/plain", "audio/mpeg", "video/mp4", "video/mpeg", "application/xml"};
	// ServerBlocks block = get_server_block(host, serverBlocks);

	// RequestDisplay();
	if (error == false)
	{
		// splite uri to scheme, authority, path, query
		UriFormat(uri, method_uri, host);
		std::string path = "/"  + uri.path;
		printf("==>%s\n", path.c_str());
		init_worker_block(worker, host, path, serverBlocks, is_dir, is_regular);
		worker.setHost(host);
		// ServerBlocks block = worker.getBlockWorker();
		std::string root = worker.getRoot();//get_root(block.getDirectives(), (std::vector<LocationsBlock>&)block.getLocations(), uri);
		std::string index = worker.getIndex();
		worker.setQuery(uri.query);
		// std::cout << "host " << host << " root " << root  << " index " << index << " path " << worker.getPath() << " query " << uri.query << std::endl;
		bool indexed = false;
		if (!worker.getLocationWorker().getPath().compare("/cgi-bin") || !worker.getLocationWorker().getPath().compare("/cgi-bin/"))
		{
			CgiEnv obj(worker);
			// obj.setPathUriVector();
			exit (1);
			std::string fullpath;
			std::string rootTmp = worker.getRoot();
			std::string pathTmp = worker.getPath();
			if (rootTmp[rootTmp.size() - 1] == '/')
				rootTmp.pop_back();
			fullpath = rootTmp+pathTmp;
			std::cout<< "location : "<<worker.getLocationWorker().getPath()<< " fullpath: "<< fullpath<< " index: " << worker.getIndex()<<" autoindex:"<< worker.getAutoIndex()<<std::endl;
			worker.setCgiStatus(true);
			exit (1);
			return;
		}
		if (is_dir == 1 && index.size() != 0)
		{
			std::string check = (worker.getRoot() + ((worker.getRoot()[worker.getRoot().size() - 1] == '/') ? "" : "/") + uri.path);
			if (access(check.c_str(), F_OK) == 0)
			{
				uri.path += index;
				indexed = true;
				is_dir = 0; is_regular = 1;
			}
		}

		// check for allowed method
		if (is_regular == 1)
		{
			int exist = F_OK;
			int rigths =(method == "POST") ? (F_OK | R_OK | W_OK):(F_OK | R_OK); 
			std::string check = (worker.getRoot() + ((worker.getRoot()[worker.getRoot().size() - 1] == '/') ? "" : "/") + uri.path);
			if (access(check.c_str(), exist) != 0)
				error = true, status = 404;
			if (error == false && access(check.c_str(), rigths) != 0)
				error = true, status = 403;
		}

		std::vector<std::string> allowedMethod = worker.getAllowMethods();
		if (allowedMethod.size() != 0)
		{
			if (find(allowedMethod.begin(), allowedMethod.end(), method) == allowedMethod.end())
				error = true, status = 405;
		}
	}
}

void	request::RequestDisplay( void )
{
	// this is for display the data stored from the request
	std::cout << "methode : " << method << ", uri " << method_uri << ", http protocol : " << http << " hostname : " << host << " error " << error << " status " << status <<std::endl;
	for (std::vector<HTTPHeader>::iterator iter = headers.begin();  iter != headers.end(); iter++)
	{
		std::cout <<"--> " << iter->name << " : ";
		std::cout << iter->values << std::endl;
		std::cout <<"Form_name: " << iter->Form_name << " , Filename " << iter->filename << ", Probable length in bits = " << iter->end - iter->begin << ", Boundary " << iter->boundry << std::endl;
		long long s = iter->begin;
		while (s > 0 && s < req.size() && req[s])
			std::cout << req[s];
		std::cout << std::endl;
	}
	std::cout << "Body : \n";
	// std::cout << body;
	std::cout << "Parsed URI : \n";
	std::cout << "Uri scheme : " << uri.scheme  << ",URI authority " << uri.authority  << ",URI path " << uri.path << ",URI query " << uri.query << ",URI fragment " << uri.fragment << std::endl;
}

request::~request()
{
	std::cout << "Request Detroyed\n";
}

request::request(const request& copy)
{
	*this = copy; 
}

request& request::operator=(const request& obj)
{
	std::cout << "Request Copied\n";
	if (this != &obj)
	{
		request_length = obj.request_length;
		method = obj.method;
		method_uri = obj.method_uri;
		uri = obj.uri;
		http = obj.http;
		host = obj.host;
		headers = obj.headers;
		body = obj.body;
		req = obj.req;
		error = obj.error;
		status = obj.status;
		is_dir = obj.is_dir;
		is_regular = obj.is_regular;
		HandleRequest  = obj.HandleRequest;
		BIndex = obj.BIndex;
		left_CR = obj.left_CR;
		NewLine = obj.NewLine;
		RequestRead = obj.RequestRead;
		ReadedFullToken = obj.ReadedFullToken;
		FillingBuffer = obj.FillingBuffer;
		Parsed_StartLine = obj.Parsed_StartLine;
		SLValidity = obj.SLValidity;
		R_Method = obj.R_Method;
		R_URI = obj.R_URI;
		R_PROTOCOL = obj.R_PROTOCOL;
		R_FUll_HEADERS = obj.R_FUll_HEADERS;
		Parsed_Header = obj.Parsed_Header;
		R_HEADER = obj.R_HEADER;
		R_VALUE = obj.R_VALUE;
		BodyLimiterType = obj.BodyLimiterType;
		R_FULL_BODY = obj.R_FULL_BODY;
		Body_Exist = obj.Body_Exist;
		Parsed_Body = obj.Parsed_Body;
		ContentLengthExist = obj.ContentLengthExist;
		ContentLengthSize = obj.ContentLengthSize;
		ChunkedRead = obj.ChunkedRead;
		ChunkedSizeRead = obj.ChunkedSizeRead;
		ChunkedSize = obj.ChunkedSize;
		ChunkSizeString = obj.ChunkSizeString;
		boundary = obj.boundary;
	}
	return (*this);
}

int request::getHeaderIndex(const std::string &name) const
{
	for(size_t i = 0; i < headers.size(); i++)
	{
		if (headers[i].name == name)
			return (i);
	}
	return (-1);
}
// Getter and Setter

std::string	const			&request::getMethod( void ) const
{
	return (method);
}

std::string	const			&request::getMethod_uri( void ) const
{
	return (method_uri);
}

t_uri const					&request::getUri( void ) const
{
	return (uri);
}

std::string	const			&request::getHttp( void ) const
{
	return (http);
}

std::string	const			&request::getHost( void ) const
{
	return (host);
}

std::vector<HTTPHeader>	const &request::getHeaders( void ) const
{
	return (headers);
}

std::string	const			&request::getBody( void ) const
{
	return (body);
}

std::string	const			&request::getReq( void ) const
{
	return (req);
}

bool					request::getError( void ) const
{
	return (error);
}

int						request::getStatus( void ) const
{
	return (status);
}

int						request::getIs_dir( void ) const
{
	return (is_dir);
}

bool							request::getRequestRead( void ) const
{
	return (RequestRead);
}

bool							request::getHandleRequest( void ) const
{
	return (HandleRequest);
}


size_t							request::getRequestLength( void ) const
{
	return (request_length);
}

int 					request::getIs_regular( void ) const
{
	return (is_regular);
}

void					request::setError(bool value)
{
	error = value;
}

void						request::setStatus(int value)
{
	status = value;
}

void							request::setRequestRead(bool value)
{
	RequestRead = value;
}
void							request::setHandleRequest(bool value)
{
	HandleRequest = value;
}
