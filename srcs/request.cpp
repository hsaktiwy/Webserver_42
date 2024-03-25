/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:15:46 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/03/24 01:11:22 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/request.hpp"
#include "../includes/cgi.hpp"

request::request(): display(false), is_indexDir(false), redirect(false), RequestRead(false), Parsed_StartLine(false), R_Method(false), R_URI(false), R_PROTOCOL(false), R_FUll_HEADERS(false),  Parsed_Header(false),  R_FULL_BODY(false), Body_Exist(false),  HandleRequest(false)
{
	BodyLimiterType = 0;
	FillingBuffer = false;
	SLValidity = false;
	error = false;
	status = 200;
	left_CR = false;
	NewLine = true;
	R_HEADER = false;
	R_VALUE = false;
	BIndex = 0;
	ChunkedSizeRead = false;
	ChunkedSize = 0;
	isCgiRequest = -1;
	is_dir = 0;
	is_regular = 0;
	maxBodySizeExist = false;
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


static	bool	getToken(char *buff, size_t &index, size_t bytes, std::string &holder, bool &boolean, bool &readStatus)
{
	while (index < bytes && buff[index] != ' ')
	{
		if (buff[index] == '\r' || buff[index] == '\n')
			return (false);
		holder += buff[index++];
		readStatus = true;
	}
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

size_t	hexaToDecimal(const std::string &str)
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

std::string		EscapedEncoding(std::string &uri, bool &error, int &status)
{
	std::string result;

	for (size_t i = 0; i < uri.size(); i++)
	{
		if (uri[i] == '%' && i + 1 < uri.size() && ishexa(uri[i + 1]))
		{
			if (i + 2 < uri.size() && ishexa(uri[i + 2]))
			{
				std::string  tmp = uri.substr(i + 1, 2);

				size_t hexa = hexaToDecimal(tmp);
				if (hexa > 31)
				{
					result += (char)hexa;
					i +=2;
				}
				else
				{
					error = true, status = 400;
					return (uri);
				}
			}
			else
				result += uri[i];
		}
		else
			result += uri[i];
	}
	return (result);
}


static void	FillUriStructor(t_uri& uri, std::string &full_uri)
{
	size_t size = 0;

	if (full_uri[size] && full_uri[size] == '/')
		size++;
	for (size_t i = size; full_uri[i] && full_uri[i] != '?'; i++)
	{
		uri.path += full_uri[i];
	}
	size += uri.path.size();
	if (full_uri[size] && full_uri[size] == '?')
		size++;
	for(size_t i = size; full_uri[i]; i++)
		uri.query += full_uri[i];
	size += uri.query.size();
}

static void UriFormat(t_uri &uri, std::string &full_uri)
{
	FillUriStructor(uri, full_uri);
}

bool	request::MethodParsing(char *buff, size_t &bytes_size, size_t &index)
{
	if (FillingBuffer == false && index < bytes_size && buff[index] == ' ')
	{
		error = true, RequestRead = true, status = 400;
		return (false);
	}
	if (!getToken(buff, index, bytes_size, method, R_Method, FillingBuffer))
	{
		error = true, RequestRead = true, status = 400;
		return (false);
	}
	return (true);
}

bool	request::UriParsing(char *buff, size_t &bytes_size, size_t &index)
{
	while (FillingBuffer == false && index < bytes_size && buff[index] == ' ')
			index++;
	if (!getToken(buff, index, bytes_size, method_uri, R_URI, FillingBuffer))
	{
		error = true, RequestRead = true, status = 400;
		return (false);
	}
	return (true);
}

bool	request::ProtocolParsing(char *buff, size_t &bytes_size, size_t &index)
{
	while (FillingBuffer == false && index < bytes_size && buff[index] == ' ')
			index++;
	while (index < bytes_size && buff[index] != ' ')
	{
		if (buff[index] == '\r' || buff[index] == '\n')
			break;
		http += buff[index++];
		FillingBuffer = true;
	}
	if (index < bytes_size && buff[index] != '\r')
	{
		error = true, RequestRead = true, status = 400;
		return (false);
	}
	else if (FillingBuffer && index < bytes_size && buff[index] == '\r')
		FillingBuffer = false, R_PROTOCOL = true;
	if ((index < bytes_size && buff[index] == '\r' &&  index + 1 < bytes_size && buff[index + 1] == '\n')
		|| ( left_CR == true && index < bytes_size && buff[index] == '\n'))
		index += (left_CR == true) ? 1 : 2, Parsed_StartLine = true, NewLine = true;
	else if (index < bytes_size && buff[index] == '\r' &&  index + 1 == bytes_size)
	{
		index++, left_CR = true;
		return (false);
	}
	else if (index < bytes_size && buff[index] == '\r' && index + 1 < bytes_size && buff[index + 1] != '\n')
	{
		error = true, RequestRead = true, status = 400;
		return (false);
	}
	return (true);
}

bool	request::StartlineParsing(char *buff, size_t &bytes_size, size_t &index)
{
	if (!R_Method)
	{
		if (!MethodParsing(buff, bytes_size, index))
			return (false);
	}
	if (R_Method && !R_URI && index < bytes_size)
	{
		if (!UriParsing(buff, bytes_size, index))
			return (false);
	}
	if (R_URI && !R_PROTOCOL && index < bytes_size)
	{
		if (!ProtocolParsing(buff, bytes_size, index))
			return (false);
	}
	if (!SLValidity && R_Method && R_URI && R_PROTOCOL)
	{
		if (method == "POST")
			Body_Exist = true;
		if (!CheckUriFormat(method_uri))
		{
			error = true, RequestRead = true, status = 400;
			return (false);
		}
		SLValidity = true;
	}
	if (left_CR == true && index < bytes_size && buff[index] == '\n')
		Parsed_StartLine = true, left_CR = false, index++,NewLine = true;
	else if (left_CR == true && index < bytes_size && buff[index] != '\n')
	{
		error = true, RequestRead = true, status = 400;
		return (false);
	}
	return (true);
}

bool	request::BaseHeadersParsing(char *buff, size_t &bytes_size, size_t &index)
{
	while (index < bytes_size)
	{
		if ((left_CR == false && buff[index] == '\r' && index + 1 < bytes_size && buff[index + 1] == '\n' && NewLine)
			|| (buff[index] == '\n' && left_CR && NewLine))
		{
			NewLine = false, R_VALUE = false, R_HEADER = false, index +=((left_CR) ? 1:2), R_FUll_HEADERS = true, left_CR = false;
			break;
		}
		if (NewLine && buff[index] == '\r')
		{
			left_CR = true, index++;
			continue;
		}
		else if (NewLine && !left_CR)
		{
			AddEmptyNode(headers), NewLine = false, R_HEADER = false, R_VALUE = false;
			continue;
		}
		if (!R_HEADER && !is_seperator(buff[index]) && !is_ctl(buff[index]))
			headers[headers.size() - 1].name += buff[index], FillingBuffer = true;
		else if (!R_HEADER && buff[index] == ':')
		{
			R_HEADER = true, index++, FillingBuffer = false;
			continue;
		}
		if (!R_HEADER && (is_seperator(buff[index]) || is_ctl(buff[index])))
		{
			error = true, RequestRead = true, status = 400;
			return (false);
		}
		if (R_HEADER && !R_VALUE && buff[index] != '\r' && !left_CR)
			headers[headers.size() - 1].values += buff[index], FillingBuffer = true;
		if (R_HEADER && !R_VALUE && (buff[index] == '\r' || left_CR))
		{
			if ((index + 1 < bytes_size && buff[index + 1] == '\n') || (left_CR && buff[index] == '\n'))
			{
				index += (left_CR) ? 1 : 2, NewLine = true, R_VALUE = true, FillingBuffer = false;
				if (left_CR)
					left_CR = !left_CR;
			}
			else if((index + 1 < bytes_size && buff[index + 1] != '\n') || (left_CR && buff[index] != '\n'))
			{
				error = true, RequestRead = true, status = 400;
				return (false);
			}
			else if (index + 1 == bytes_size && !left_CR)
				index += 1, left_CR = true;
			continue;
		}
		index++;
	}
	return (true);
}

bool	request::IdentifieHost( void )
{
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
		return (false);
	}
	return (true);
}

void	request::BodyDelimiterIdentification( void )
{
	int CLIndex = getHeaderIndex("Content-Length");
	if (CLIndex != -1)
	{
		ContentLengthSize = ft_atoll(headers[CLIndex].values.c_str());
		BodyLimiterType = 1;
	}
	int CTindex = getHeaderIndex("Content-Type");
	if (CTindex != -1)
	{
		std::string Format = "multipart/form-data;";
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
	int TEindex = getHeaderIndex("Transfer-Encoding");
	if (TEindex != -1)
	{
		std::string Format = "chunked";
		size_t pos = headers[TEindex].values.find(Format);
		if (pos != std::string::npos)
			BodyLimiterType = 2;
	}
}

bool	request::HeadersParsing(std::vector<ServerBlocks> &serverBlocks, Worker& worker, char *buff, size_t &bytes_size, size_t &index,int fd,std::map<int, int> &matched_server_block)
{
	if (!R_FUll_HEADERS)
	{
		if (!BaseHeadersParsing(buff, bytes_size, index))
			return (false);
	}
	if (R_FUll_HEADERS)
	{
		if (!IdentifieHost())
			return (false);
		BodyDelimiterIdentification();
		UriFormat(uri, method_uri);
		uri.path = EscapedEncoding(uri.path, error, status);
		uri.query = EscapedEncoding(uri.query, error, status);
		std::string path = "/"  + uri.path;
		if (http != "HTTP/1.1")
		{
			error = true, status = 505, RequestRead = true;
			return (false);
		}
		if (check_uri(path) == false)
		{
			error = true, status = 400, RequestRead = true;
			return (false);
		}
		init_worker_block(worker, host, path, serverBlocks, is_dir, is_regular,fd,matched_server_block);
		if (worker.get_max_body_size() != "")
		{
			maxBodySizeExist = true;
			max_body_size = std::atoi(worker.get_max_body_size().c_str());
		}
		else
		{
			maxBodySizeExist = true;
			max_body_size = 1000000;
		}
		worker.setHost(host);
		Parsed_Header = true;
		FillingBuffer = false;
		left_CR = false;
	}
	return (true);
}

bool	request::BodyIdentifiedByContentLength(char *buff, size_t &bytes_size, size_t &index)
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
		return (false);
	}
	return (true);
}

void	request::BodyIdentifiedByTransfertEncoding(char *buff, size_t &bytes_size, size_t &index)
{
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
				index += (left_CR) ? 1 : 2, ChunkedSizeRead = false, FillingBuffer = false, left_CR = false;
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

void	request::BodyIdentifiedByMultFormData(char *buff, size_t &bytes_size, size_t &index)
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

bool	request::BodyParsing(char *buff, size_t &bytes_size, size_t &index)
{
	if (BodyLimiterType == 0)
	{
		RequestRead = true;
	}
	else if (BodyLimiterType == 1)
	{
		if (!BodyIdentifiedByContentLength(buff, bytes_size, index))
			return (false);
	}
	else if (BodyLimiterType == 2)
	{
	
		BodyIdentifiedByTransfertEncoding(buff, bytes_size, index);
	}
	else if (BodyLimiterType == 3)
	{
		BodyIdentifiedByMultFormData(buff, bytes_size, index);
	}

	if	(maxBodySizeExist)
	{
		if (body.size() > max_body_size)
		{
			error = true, RequestRead = true,status = 413;
			return (false);
		}
	}
	return (true);
}

void	request::ParseRequest(std::vector<ServerBlocks> &serverBlocks, std::map<int, int> &matched_server_block , Worker& worker, char *buff, size_t bytes_size, int fd)
{
	size_t index = 0;
	if (!Parsed_StartLine)
	{
		if (!StartlineParsing(buff, bytes_size, index))
			return ;
	}
	if (Parsed_StartLine && !Parsed_Header && index < bytes_size)
	{
	
		if (!HeadersParsing(serverBlocks, worker, buff, bytes_size, index,fd,matched_server_block))
			return ;
	}
	if (Parsed_StartLine && Parsed_Header && Body_Exist)
	{
		if (!BodyParsing(buff, bytes_size, index))
			return ;
		
	}
	else if (Parsed_StartLine && Parsed_Header)
	{
		RequestRead = true;
	}
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

void	AllowedMethod(Worker& worker, std::string &method, bool &error, int &status)
{
	std::string allowedMethod_s[] = {"POST", "DELETE", "GET"};
	bool supported = false, supported2 = false;
	for(size_t i = 0; i < 3; i++)
	{
		if (method == allowedMethod_s[i])
		{
			supported = true;
			break;
		}
	}
	std::vector<std::string> allowedMethods = worker.getAllowMethods();
	if (error == false && allowedMethods.size() != 0)
	{
		if (find(allowedMethods.begin(), allowedMethods.end(), method) != allowedMethods.end())
			supported2 = true;
	}
	else if (error == false && allowedMethods.size() == 0)
		supported2 = true;
	if (error == false && (supported == false || supported2 == false))
		error = true, status = 405;
}

void IndexingtoIndex(Worker& worker, int &is_dir, int &is_regular, t_uri& uri, bool &error, bool &redirect, bool &is_indexDir)
{
	std::string index = worker.getIndex();
	if (error == false && is_dir == 1 && uri.path.size() > 0 && uri.path[uri.path.size() - 1] != '/')
	{
		redirect = true;
		return ;
	}
	if (error == false && is_dir == 1 && index.size() != 0)
	{
		std::string check = (worker.getRoot() + "/" + uri.path);
		check = NormilisePath(check);
		if (access(check.c_str(), F_OK) == 0)
		{
			uri.path += "/" +index;
			check += "/" +index;
			if (Is_Directory(check) == 0)
				is_indexDir = true;
			else
				is_dir = 0, is_regular = 1;
		}
	}
}

void	FileAccessingRigth(Worker& worker, t_uri& uri, bool &error, int &status, int &is_regular, std::string &method)
{
	if (error == false && is_regular == 1)
	{
		int rigths = (method == "POST") ? F_OK : F_OK | R_OK;
		std::string check = (worker.getRoot() + ((worker.getRoot()[worker.getRoot().size() - 1] == '/') ? "" : "/") + uri.path);
		int r_acceess = access(check.c_str(), rigths);
		if (r_acceess != 0)
			(errno == EACCES) ? (error = true, status = 403) : (error = true, status = 404);
	}
}

bool request::isCgiLocationMatched(Worker &worker)
{
    size_t found = worker.getLocationWorker().getPath().find("/cgi-bin/");

	if (found != std::string::npos)
    {
        if (found == 0 )
            return true;
        else
            return false;
    }
    return false;
}


void	request::CheckRequest(Worker& worker, bool &cgiStat)
{
	if (error == false)
	{
		if (error == false && worker.getRoot().empty())
			error = true, status = 404;
		AllowedMethod(worker, method, error, status);
		worker.setQuery(uri.query);
		if (!worker.getLocationWorker().getPath().compare("/cgi-bin") || !worker.getLocationWorker().getPath().compare("/cgi-bin/") || isCgiLocationMatched(worker))
		{
			worker.setCgiStatus(true);
			isCgiRequest = 1;
			cgiStat = true;
			return;
		}
		else
		{
			isCgiRequest = 0;
			if (method == "GET")
				IndexingtoIndex(worker, is_dir, is_regular, uri, error, redirect, is_indexDir);
			FileAccessingRigth(worker, uri, error, status, is_regular, method);
		}
	}
}

int	request::getHeaderValue(const std::string &header,std::string &buffer)
{
	int index = getHeaderIndex(header);

	if (index != -1)
	{
		buffer = headers[index].values;
		return (1);
	}
	return (0);
}

request::~request()
{

}

request::request(const request& copy)
{
	*this = copy; 
}

request& request::operator=(const request& obj)
{
	if (this != &obj)
	{
		is_indexDir = obj.is_indexDir;
		redirect = obj.redirect;
		method = obj.method;
		method_uri = obj.method_uri;
		uri = obj.uri;
		http = obj.http;
		host = obj.host;
		headers = obj.headers;
		body.clear();
		body = obj.body;
		error = obj.error;
		status = obj.status;
		is_dir = obj.is_dir;
		is_regular = obj.is_regular;
		HandleRequest  = obj.HandleRequest;
		BIndex = obj.BIndex;
		left_CR = obj.left_CR;
		NewLine = obj.NewLine;
		RequestRead = obj.RequestRead;
		FillingBuffer = obj.FillingBuffer;
		Parsed_StartLine = obj.Parsed_StartLine;
		display = obj.display;
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
		ContentLengthSize = obj.ContentLengthSize;
		ChunkedSizeRead = obj.ChunkedSizeRead;
		ChunkedSize = obj.ChunkedSize;
		ChunkSizeString = obj.ChunkSizeString;
		boundary = obj.boundary;
		is_dir = obj.is_dir;
		is_regular = obj.is_regular;
		maxBodySizeExist = obj.maxBodySizeExist;
		max_body_size = obj.max_body_size;
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

bool							request::isRedirect( void )
{
	return (redirect);
}

std::string const			&request::getBoundary( void ) const
{
	return boundary;
}
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

int ::request::getCgiStatus() const
{
	return isCgiRequest;
}

bool 							request::getStartLine( void ) const
{
	return (Parsed_StartLine);
}

bool							request::getDisplay( void ) const
{
	return (display);
}

void							request::setDisplay(bool value)
{
	display = value;
}
void							request::setCgiStatus(int value)
{
	isCgiRequest = value;
}

bool 						request::isIndexDir( void )
{
	return (is_indexDir);
}
