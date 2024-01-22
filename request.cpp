#include "request.hpp"

request::request()
{

}

static bool CharacterUri(char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ':'
		|| (c == '0' && c == '9') ||   c == '/' || c == ';' || c == '?'
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

// using this fucntion we can avoid adding the SP and HT plus LWS to our request string and replace them all bye a space
static void replaceConsecutiveSpaces(std::string &HTTPrequest, std::string &req)
{
	int			i;
	bool		space = false;
	char		quoted = '\0';

	i = 0;
	while (req[i])
	{
		if (quoted == '\0' && (req[i] == SP || req[i] == HT) && space == false)
		{
			if (space == false)
			{
				HTTPrequest += " ";
				space = true;
			}
		}
		else if (quoted == '\0' && req[i] == CR && req[i + 1] && req[i + 1] == LF
			&& req[i + 2] && (req[i + 2] == SP || req[i + 2] == HT))
		{
			if (space == false)
			{
				HTTPrequest += " ";
				space = true;
			}
		}
		else if (quoted == '\0' && (req[i] == '\"' || req[i] == '\''))
		{
			HTTPrequest += req[i];
			quoted = req[i];
			space = false;
		}
		else if (quoted != '\0' && quoted == req[i])
		{
			HTTPrequest += req[i];
			quoted = '\0';
			space = false;
		}
		else
		{
			HTTPrequest += req[i];
			space = false;
		}
		i++;
	}
}
// Host: 127.0.0.1:8000\r\nConnection: 
std::string spliter(std::string &str, const std::string & delimiter, size_t &index)
{
	size_t s = ((index == 0) ? 0 : delimiter.size());
	size_t pos = str.find(delimiter, (index + s < str.size()) ? (index + s) : str.size() - 1);
	std::string line;

	if (pos != std::string::npos)
	{
		line = str.substr(index + s, pos - (index + s));
		index = pos;
	}
	else
		line = str.substr(index + s), index = std::string::npos;
	return (line);
}

static bool CheckForBody(std::string &request, size_t index)
{
	if (request[index] && request[index] == '\r'
		&& request[index + 1] && request[index + 1] == '\n'
		&& request[index + 2] && request[index + 2] == '\r'
		&& request[index + 3] && request[index + 3] == '\n')
			return (true);
	return (false);
}

static void	ParseHeaders(std::vector<HTTPHeader> &headers, std::string &request, std::string &body, size_t &index, bool &error, int &status)
{
	std::string tmp;
	std::string delimiter = "\r\n";
	bool stop = false;

	while (!stop)
	{
		if (index != std::string::npos)
		{
			HTTPHeader 	header;
			size_t i = 0;
			// get a header
			if (!CheckForBody(request, index))
			{
				tmp = spliter(request, delimiter, index);
				if (tmp.size() == 0)
					continue;
				// surpace space if it was at first i think this was allowed
				if (tmp[i] == ' ')
					i++;
				while (tmp[i] && tmp[i] != ' ' && tmp[i] != ':')
					header.name += tmp[i++];
				if (tmp[i] == ' ')
					i++,error = true, status = 400;
				// check for the known header then define it rigth values syntaxe
				if (tmp[i] != ':')
					error = true, status = 400;
				else
					i++;
				std::string	value;
				if (tmp[i] == ' ')
					i++;
				// should i handle seperators ? 
				while (tmp[i])
				{
					if (tmp[i] == ' ')
					{
						if (value.size() > 0)
							header.values.push_back(value), value = "";
						i++;
					}
					value += tmp[i++];
					if (!tmp[i])
						if (value.size() > 0)
							header.values.push_back(value), value = "";
				}
				headers.push_back(header);
			}
			else
			{
				body = &request[index + 4];
				stop = true;
			}
		}
		else
			stop = true;
	}
}

static void	MethodParsing(bool &error, int &status, std::string &HTTPrequest, std::string &method, std::string &uri, std::string &http, size_t &index)
{
	std::set<std::string> ValideMethodes; 
	ValideMethodes.insert("GET");
	ValideMethodes.insert("DELETE");
	ValideMethodes.insert("POST");
	// Define the methode
	std::string tmp = spliter(HTTPrequest, "\r\n", index);
	method = "";
	size_t i = 0;
	if (tmp[0] && !std::isalpha(tmp[0]))
		error = true,status = 400; //  bad request
	// get the methode
	while (tmp[i] && tmp[i] != ' ' && error == false)
	{
		if (std::isalpha(tmp[i]))
			method += tmp[i];
		else
			error = true,status = 400; //  bad request
		i++;
	}
	// check if the method is valide
	if (error == false)
	{
		std::set<std::string>::iterator iter = ValideMethodes.find(method);
		if (iter == ValideMethodes.end())
			error = true,status = 405;// is there another way to say undefine ?(501 : Not Implemented, if the method is unrecognized or not implemented by the origin server) 
	}
	// get the uri
	if (tmp[i] == ' ')
		i++;
	uri = "";
	while (tmp[i] && tmp[i] != ' ' && error == false)
			uri += tmp[i++];
	// check the validity of the uri
	//  			?
	// check the http protocol
	if (!CheckUriFormat(uri))
		error = true, status = 400;
	if (tmp[i] == ' ')
		i++;
	while (tmp[i] && tmp[i] != ' ')
		http += tmp[i++];

	// check the http protocol
	if (http != "HTTP/1.1")
		error = true,status = 400;
}

static void	GetRequestHost(std::vector<HTTPHeader> &headers, std::string &host)
{
	for(std::vector<HTTPHeader>::iterator iter = headers.begin(); iter != headers.end(); iter++)
	{
		if (iter->name == "Host")
		{
			host = iter->values.front();
			return ;
		}
	}
}
void	request::ParseRequest(char *r)
{
	req = r;
	std::string HTTPrequest;
	// HTTPHeader HTTPHeaders;

	error = false;
	status = 200;
	size_t index = 0;
	// Turn any none quoted  *SP or HT or LWS to one space
	// std::cout << "Before Request form :\n" << req <<std::endl;
	replaceConsecutiveSpaces(HTTPrequest, req);
	// Define the method  ?
	MethodParsing(error, status, HTTPrequest, method, method_uri, http, index);
	// parsthe headers
	ParseHeaders(headers, req, body, index, error, status);
	GetRequestHost(headers, host);
	// files, plus check the syntaxe
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
	if (absoluteUri == true)
	{
		// in case if the uri is absolute
		if (full_uri.find("http:://") == 0)
		{
			// extract the shceme
			uri.scheme = "http:://";
			FillUriStructor(uri, full_uri, true);
		}
		else
		{
			// extract the shceme
			uri.scheme = "https:://";
			FillUriStructor(uri, full_uri, true);
		}
		uri.type = ABSOLUTE;
	}
	else if (full_uri.find(host) == 0)
		FillUriStructor(uri, full_uri, true), uri.type = AUTHORITY;
	else
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

static bool	CheckPathExistance(t_uri &uri, std::string root)
{
	// DIR *dir;
	// struct dirent *dirent;
	std::string src,path;
	bool is_Directory = (uri.path[uri.path.size() - 1] == '/') ? true : false;
	std::string file;
	src = ((root[0] == '/') ? "" : "/") + root + ((root[root.size() - 1] == '/') ? "" : "/") + uri.path;
	struct stat *statbuff = NULL;
	std::cout << src << std::endl;
	int res = access(src.c_str(), F_OK | R_OK);

	std::cout << " res " << res << std::endl;
	if (res == 0)
		return (true);
	// path = src;
	// size_t s = path.rfind("/");
	// if (s != std::string::npos)
	// 	path = path.substr(0, s);
	// std::cout << " Path " << path << std::endl;
	// dir = opendir(path.c_str());
	// if (dir)
	// {
	// 	if (s != std::string::npos)
	// 	{
	// 		std::string file = src.substr(s + 1);
	// 		bool stop = false;
	// 		while (!stop)
	// 		{
	// 			dirent = readdir(dir);
	// 			if (dirent)
	// 			{
	// 				std::cout << dirent->d_name << std::endl;
	// 				if (is_Directory && dirent->d_type == DT_DIR)
	// 				{
	// 					std::string tmp = file;
	// 					tmp[tmp.size() - 1] = '\0';
	// 					if (ft_strcmp(dirent->d_name, file.c_str()))
	// 					{
	// 						if (closedir(dir) == -1)
	// 							std::cerr << "Error \n	closedir Failed" << std::endl;
	// 						return (true);
	// 					}
	// 				}
	// 				else
	// 				{
	// 					if (ft_strcmp(dirent->d_name, file.c_str()))
	// 					{
	// 						if (closedir(dir) == -1)
	// 							std::cerr << "Error \n	closedir Failed" << std::endl;
	// 						return (true);
	// 					}
	// 				}
	// 			}
	// 			else
	// 				stop = true;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if (closedir(dir) == -1)
	// 			std::cerr << "Error \n	closedir Failed" << std::endl;
	// 		return (true);
	// 	}
	// }
	return (false);
}

std::string	get_root(std::vector<Directives> &directives, std::vector<LocationsBlock>& locations, t_uri& uri)
{
	std::string root;
	std::string locationPrimeRoot;// the prime location  root is the root that in the location block that have '/' as it path
	std::string locationRoot;
	size_t	s = 0;
	// this will try to search for the longest matchs Ex: path :/test/hello/w/fil.txt, and we have 3 locations with paths
	// as follow /test, /test/hello/w, /test/hello

	// check if the Directive had a directly root, if it has suppose it is location is / 
	for (std::vector<Directives>::iterator iter = directives.begin(); iter != directives.end(); iter++)
	{
		if (iter->getDirective() == "root")
		{
			root = iter->getArgument()[0];
			break;
		}
	}
	//  check for root in location
	for(std::vector<LocationsBlock>::iterator iter = locations.begin(); iter != locations.end(); iter++)
	{
		std::string path = iter->getPath();
		size_t size = uri.path.find(path);
		if (path == "/" || size != std::string::npos)
		{
			for (std::vector<Directives>::iterator it = iter->getDirectives().begin(); it != iter->getDirectives().end(); it++)
			{
				if (it->getDirective() == "root")
				{
					if (path == "/")
						locationPrimeRoot = it->getArgument()[0];
					else
					{
						if (size > s)
							locationRoot = it->getArgument()[0], s = size;
					}
				}
			}
		}
	}
	// new let us choose our root
	if (locationRoot != "")
		return (locationRoot);
	if (locationPrimeRoot != "")
		return (locationPrimeRoot);
	return (root);
}

void	request::CheckRequest(std::vector<ServerBlocks> &serverBlocks, Worker& worker)
{
	is_dir = 0;
	is_regular = 0;
	std::string KnownHeaders[] = {"Host", "Accept", "Accept-Language", "Accept-Encoding", "Connection", "Referer"};
	std::string mimeType[] = {"image/avif", "image/avif", "image/jpeg", "image/gif", "image/png", "text/csv",  "text/html",   "text/javascript", "text/plain", "text/xml", "text/plain", "audio/mpeg", "video/mp4", "video/mpeg", "application/xml"};
	// ServerBlocks block = get_server_block(host, serverBlocks);
	if (error == false)
	{
		// splite uri to scheme, authority, path, query
		UriFormat(uri, method_uri, host);
		init_worker_block(worker, host, uri.path, serverBlocks, is_dir, is_regular);
		// ServerBlocks block = worker.getBlockWorker();
		std::string root = worker.getRoot();//get_root(block.getDirectives(), (std::vector<LocationsBlock>&)block.getLocations(), uri);
		std::string index = worker.getIndex();
		std::cout << "host " << host << " root " << root  << " index " << index << std::endl;
		if (uri.path.size() == 0 && index.size() != 0)
			uri.path += index;
		std::cout << "New Path " << uri.path << std::endl;
		// check for allowed method
		std::vector <std::string> allowedMethod = worker.getAllowMethods();
		if (allowedMethod.size() != 0)
		{
			if (find(allowedMethod.begin(), allowedMethod.end(), method) == allowedMethod.end())
				error = true, status = 405;
		}
	}
	RequestDisplay();
}

void	request::RequestDisplay( void )
{
	std::cout << "methode : " << method << ", uri " << method_uri << ", http protocol : " << http << " hostname : " << host << " error " << error << " status " << status <<std::endl;
	for (std::vector<HTTPHeader>::iterator iter = headers.begin();  iter != headers.end(); iter++)
	{
		std::cout <<"--> "<< iter->name << " : ";
		for(std::list<std::string>::iterator it = iter->values.begin(); it != iter->values.end(); it++)
		{
			std::cout << *it << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "Body : \n";
	std::cout << body;
	std::cout << "Parsed URI : \n";
	std::cout << "Uri scheme : " << uri.scheme  << ",URI authority " << uri.authority  << ",URI path " << uri.path << ",URI query " << uri.query << ",URI fragment " << uri.fragment << std::endl;
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

	}
	return (*this);
}

// Getter and Setter

std::string				&request::getMethod( void )
{
	return (method);
}

std::string				&request::getMethod_uri( void )
{
	return (method_uri);
}

t_uri					&request::getUri( void )
{
	return (uri);
}

std::string				&request::getHttp( void )
{
	return (http);
}

std::string				&request::getHost( void )
{
	return (host);
}

std::vector<HTTPHeader>	&request::getHeaders( void )
{
	return (headers);
}

std::string				&request::getBody( void )
{
	return (body);
}

std::string				&request::getReq( void )
{
	return (req);
}

bool					request::getError( void )
{
	return (error);
}

int						request::getStatus( void )
{
	return (status);
}

int						request::getIs_dir( void )
{
	return (is_dir);
}

int 					request::getIs_regular( void )
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
