/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:15:46 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/02/27 16:46:44 by aalami           ###   ########.fr       */
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
	NewLine = true;
	R_HEADER = false;
	R_VALUE = false;
	BIndex = 0;
	ChunkedRead = false;
	ChunkedSizeRead = false;
	ChunkedSize = 0;
	isCgiRequest = false;
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

// static bool absoluteURI(std::string &uri)
// {
// 	if (uri.size() > 4)
// 	{
// 		bool http = (uri.find("http:://") == 0) ? true : false, https = (uri.find("http:://") == 0) ? true : false;
// 		if (http || https)
// 			return (true);
// 	}
// 	return (false);
// }

// bool is_query(char c)
// {
// 	if (c == ';' ||  c == '/' ||  c == '?' ||  c == ':' ||  c == '@'
// 		||  c == '&' ||  c == '=' ||  c == '+' || c == '$' || c == ','
// 		|| std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '!'
// 		|| c == '~' || c == '*' || c == '\'' || c == '(' || c == ')')
// 		return (true);
// 	return (false);
// }

static void	FillUriStructor(t_uri& uri, std::string &full_uri)// authority boolean will confirme if there is a authority in the uri or note
{
	size_t size = 0;
	// size += uri.scheme.size(); 
	// extract the authority
	if (full_uri[size] && full_uri[size] == '/')
		size++;
	printf("size %lu index[%lu] = %c\n", size, size, full_uri[size]);
	// extract the path
	for (size_t i = size; full_uri[i] && full_uri[i] != '?'; i++)
	{
		uri.path += full_uri[i];
	}
	size += uri.path.size();
	if (full_uri[size] && full_uri[size] == '?')
		size++;
	// extract the query
	for(size_t i = size; full_uri[i]; i++)
		uri.query += full_uri[i];
	size += uri.query.size();
}

static void UriFormat(t_uri &uri, std::string &full_uri, std::string &host)// 1 for absolute 2 for relative 3 for autority 
{
	// check it the uri start with http or https
	//[scheme]://[authority]/[path]?[query]#[fragment]
	// bool	absoluteUri = absoluteURI(full_uri);
	FillUriStructor(uri, full_uri);
}

bool	request::MethodParsing(char *buff, ssize_t &bytes_size, size_t &index)
{
	// check if the buff start with sapce this case is not valide at all
	if (FillingBuffer == false && index < bytes_size && buff[index] == ' ')
	{
		error = true, RequestRead = true, status = 400;
		return (false);
	}
	// new let us get our method if it possible
	if (!getToken(buff, index, bytes_size, method, R_Method, FillingBuffer))
	{
		error = true, RequestRead = true, status = 400;
		return (false);
	}
	return (true);
}

bool	request::UriParsing(char *buff, ssize_t &bytes_size, size_t &index)
{
	// i will try to avoid all spaces
	while (FillingBuffer == false && index < bytes_size && buff[index] == ' ')
			index++;
	// now let try to get the Fully URI
	if (!getToken(buff, index, bytes_size, method_uri, R_URI, FillingBuffer))
	{
		error = true, RequestRead = true, status = 400;
		return (false);
	}
	return (true);
}

bool	request::ProtocolParsing(char *buff, ssize_t &bytes_size, size_t &index)
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
		return (false);
	}
	else if (FillingBuffer && index < bytes_size && buff[index] == '\r')
		FillingBuffer = false, R_PROTOCOL = true;
	// this will try to check for the end of our startline (and it must be ended with \r\n)
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

bool	request::StartlineParsing(char *buff, ssize_t &bytes_size, size_t &index)
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
		printf("Supper_be\n");
		error = true, RequestRead = true, status = 400;
		return (false);
	}
	return (true);
}

bool	request::BaseHeadersParsing(char *buff, ssize_t &bytes_size, size_t &index)
{
	while (index < bytes_size)
	{
		// when i should create a header ? when i have flag saying that i passed new line
		// when i need to stop this shite when i get new line the annalyse a new line after
		// added
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

bool	request::IdentifieHost(char *buff, ssize_t &bytes_size, size_t &index)
{
	// trim all values
	for(size_t i = 0; i < headers.size(); i++)
	{
		headers[i].values = ft_trim(headers[i].values, " ");
	}

	// geting host value
	int HOSTindex = getHeaderIndex("Host");
	if (HOSTindex != -1)
	{
		int i = 0;
		host = &headers[HOSTindex].values[i];
	}
	else
	{
		// printf("IdentifieHost\n");
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
}

bool	request::HeadersParsing(std::vector<ServerBlocks> &serverBlocks, Worker& worker, char *buff, ssize_t &bytes_size, size_t &index)
{
	if (!R_FUll_HEADERS)
	{
		if (!BaseHeadersParsing(buff, bytes_size, index))
			return (false);
		// when i need stop the header insertion, when i passed new line
	}
	if (R_FUll_HEADERS)
	{
		// Identifie our host , plus trim all headers values
		if (!IdentifieHost(buff, bytes_size, index))
			return (false);
		// identifie the body delimiter is it Content-Length,Transfer-encoding,Content-Type, or even all of them 
		BodyDelimiterIdentification();
		// uri secondary parsing 
		method_uri = EscapedEncoding(method_uri, error, status);
		UriFormat(uri, method_uri, host);
		std::string path = "/"  + uri.path;
		if (http != "HTTP/1.1")
		{
			error = true, status = 505, RequestRead = true;
			return (false);
		}
		// initialize our worker init base one our uri parsing result and host identifying
		printf("%s _ %s\n", method_uri.c_str(), path.c_str());
		init_worker_block(worker, host, path, serverBlocks, is_dir, is_regular);
		printf("is_regular %d is_dir %d\n", is_regular, is_dir);
		// exit(0);
		// check for max body size existing and it value
		if (worker.get_max_body_size() != "")
		{
			maxBodySizeExist = true;
			max_body_size = std::atoi(worker.get_max_body_size().c_str());
		}
		else
		{
			maxBodySizeExist = true;
			max_body_size = 1000000;// by default we will allow 1MB == (1*10^6)B
		}
		worker.setHost(host);
		Parsed_Header = true;
		FillingBuffer = false;
		left_CR = false;
		// printf("Does the bodyExist %d, and what delimiter whe need %d\n", Body_Exist, BodyLimiterType);
	}
	return (true);
}

bool	request::BodyIdentifiedByContentLength(char *buff, ssize_t &bytes_size, size_t &index)
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

void	request::BodyIdentifiedByTransfertEncoding(char *buff, ssize_t &bytes_size, size_t &index)
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

void	request::BodyIdentifiedByMultFormData(char *buff, ssize_t &bytes_size, size_t &index)
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

bool	request::BodyParsing(char *buff, ssize_t &bytes_size, size_t &index)
{
	if (BodyLimiterType == 0)
	{
		// in case there is no bady identifier for our case it mean no bady at all
		RequestRead = true;
	}
	else if (BodyLimiterType == 1)
	{
		if (!BodyIdentifiedByContentLength(buff, bytes_size, index))
			return (false);
	}
	else if (BodyLimiterType == 2)
	{
		// first i need to define the size of the chunked
		BodyIdentifiedByTransfertEncoding(buff, bytes_size, index);
	}
	else if (BodyLimiterType == 3)
	{
		BodyIdentifiedByMultFormData(buff, bytes_size, index);
	}
	// check for max size limitation if it exist in our http
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
void	request::ParseRequest(std::vector<ServerBlocks> &serverBlocks, Worker& worker, char *buff, ssize_t bytes_size)
{
	// std::string allowedMethod[] = {"POST", "GET", "DELETE"};
	size_t index = 0;
	// std::cout << body.size() << std::endl;
	if (!Parsed_StartLine)
	{
		// printf("Test1\n");
		if (!StartlineParsing(buff, bytes_size, index))
			return ;
		// if (R_Method && R_URI && R_PROTOCOL)
		// 	printf("Start line  : %s %s %s\n", method.c_str(), method_uri.c_str(), http.c_str());
	}
	// check if the header is parsed
	if (Parsed_StartLine && !Parsed_Header && index < bytes_size)
	{
		// printf("Test2\n");
		// printf("Test2, %d\n", FillingBuffer);
		// header parsing
		if (!HeadersParsing(serverBlocks, worker, buff, bytes_size, index))
			return ;
	}
	// check the body existance the read and define the end of it
	if (Parsed_StartLine && Parsed_Header && Body_Exist)
	{
		// printf("Test3\n");
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

	// this will check for our static supported methods
	for(size_t i = 0; i < 3; i++)
	{
		if (method == allowedMethod_s[i])
		{
			supported = true;
			break;
		}
	}

	// this will check if our configue has another idea
	std::vector<std::string> allowedMethods = worker.getAllowMethods();
	printf("%lu size\n", allowedMethods.size());
	if (error == false && allowedMethods.size() != 0)
	{
		printf("%d", find(allowedMethods.begin(), allowedMethods.end(), method) != allowedMethods.end());
		if (find(allowedMethods.begin(), allowedMethods.end(), method) != allowedMethods.end())
			supported2 = true;
	}
	else if (error == false && allowedMethods.size() == 0)
		supported2 = true;
	if (supported == false || supported2 == false)
		error = true, status = 405;
}

bool	IndexingtoIndex(Worker& worker, int &is_dir, int &is_regular, t_uri& uri, bool &error)
{
	std::string index = worker.getIndex();

	if (error == false && is_dir == 1 && index.size() != 0)
	{
		std::string check = (worker.getRoot() + ((worker.getRoot()[worker.getRoot().size() - 1] == '/') ? "" : "/") + uri.path);
		if (access(check.c_str(), F_OK) == 0)
		{
			uri.path += index;
			is_dir = 0; is_regular = 1;
			return (true);
		}
	}
	return (false);
}

void	FileAccessingRigth(Worker& worker, t_uri& uri, bool &error, int &status, int &is_regular, std::string &method)
{
	printf("error  %d status %d regular %d\n", error, status, is_regular);
	if (error == false && is_regular == 1)
	{
		int rigths = (method == "POST") ? F_OK : F_OK | R_OK;
		std::string check = (worker.getRoot() + ((worker.getRoot()[worker.getRoot().size() - 1] == '/') ? "" : "/") + uri.path);
		int r_acceess = access(check.c_str(), rigths);
		// printf("access rigth (path : %s): %d\n", check.c_str(), r_acceess);
		if (r_acceess != 0)
			(errno == EACCES) ? (error = true, status = 403) : (error = true, status = 404);
	}
	// exit(0);
}

void	request::CheckRequest(std::vector<ServerBlocks> &serverBlocks, Worker& worker)
{
	// RequestDisplay();
	if (error == false)
	{
		// chekc if the method is supported bye the server
		AllowedMethod(worker, method, error, status);
		// std::string root = worker.getRoot();//get_root(block.getDirectives(), (std::vector<LocationsBlock>&)block.getLocations(), uri);
		worker.setQuery(uri.query);
		// std::cout << "host " << host << " root " << root  << " index " << index << " path " << worker.getPath() << " query " << uri.query << std::endl;
		if (!worker.getLocationWorker().getPath().compare("/cgi-bin") || !worker.getLocationWorker().getPath().compare("/cgi-bin/"))
		{
			
			worker.setCgiStatus(true);
			isCgiRequest = true;
			return;
		}
		// static level
		// check for index existing in our location or root
		bool indexed = IndexingtoIndex(worker, is_dir, is_regular, uri, error);
		// if the path is file check it existence and access rigth
		FileAccessingRigth(worker, uri, error, status, is_regular, method);
		// printf("error %d, status %d\n", error, status);
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
	std::cout << "Uri scheme : " << ",URI authority " << uri.authority  << ",URI path " << uri.path << ",URI query " << uri.query << std::endl;
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
	// std::cout << "Request Detroyed\n";
}

request::request(const request& copy)
{
	*this = copy; 
}

request& request::operator=(const request& obj)
{
	// std::cout << "Request Copied\n";
	if (this != &obj)
	{
		request_length = obj.request_length;
		method = obj.method;
		method_uri = obj.method_uri;
		uri = obj.uri;
		http = obj.http;
		host = obj.host;
		headers = obj.headers;
		body.clear();
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
// Getter and Setter
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
bool ::request::getCgiStatus() const
{
	return isCgiRequest;
}
