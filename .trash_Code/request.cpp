
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
		uri.type = ABSOLUTE;
	}
	else if (full_uri.find(host) == 0)
		FillUriStructor(uri, full_uri, true), uri.type = AUTHORITY;
	else
		FillUriStructor(uri, full_uri, false), uri.type = RELATIVE;
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

static void	CheckPathExistance()
{

}

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

void							request::AddToRawRequest(char *buff, ssize_t bytes_read)
{
	printf("request_length %lu\n", request_length);
	request_length += bytes_read;
	for(ssize_t i = 0; i < bytes_read; i++)
	{
		req.push_back(buff[i]);
		std::cout << buff[i];
	}
		
}
/*begin*/
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
	while (tmp[i] && tmp[i] != ' ')
			uri += tmp[i++];
	// check the validity of the uri
	//  			?
	// check the http protocol
	if (error ==  false && !CheckUriFormat(uri))
		error = true, status = 400;
	if (tmp[i] == ' ')
		i++;
	while (tmp[i] && tmp[i] != ' ')
		http += tmp[i++];

	// check the http protocol
	if (error == false && http != "HTTP/1.1")
		error = true,status = 400;
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
	std::string boundary;
	bool stop = false;

	while (!stop)
	{
		if (index != std::string::npos)
		{
			HTTPHeader 	header;
			header.end = header.begin = -1;
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
					value += tmp[i++];
				// store the value of the header
				header.values = value;
				// after all this let us now identifie the header with the name of Content that have the multipart/form-data
				if (header.name == "Content-Type")
				{
					std::string Format = "multipart/form-data;";
					size_t pos = header.values.find(Format);
					if (pos == 0)
					{
						std::string express = "boundary=";
						size_t pos2 = header.values.find(express);
						if (pos2 != std::string::npos && pos2 > pos + Format.size())
						{
							pos2 += express.size();
							ExtractValues(header.values, boundary, pos2);
							header.boundry = boundary;
						}
					}
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

static void	GetRequestHost(std::vector<HTTPHeader> &headers, std::string &host)
{
	for(std::vector<HTTPHeader>::iterator iter = headers.begin(); iter != headers.end(); iter++)
	{
		if (iter->name == "Host")
		{
			host = iter->values;
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
	std::cout << "Before Request form :\n" << req <<std::endl;
	replaceConsecutiveSpaces(HTTPrequest, req);
	// Define the method  ?
	// printf("><<><><><%d %d\n", error, status);
	MethodParsing(error, status, HTTPrequest, method, method_uri, http, index);
	// printf("><<><><><%d %d\n", error, status);

	// parsthe headers
	ParseHeaders(headers, req, body, index, error, status);
	// printf("><<><><><%d %d\n", error, status);

	GetRequestHost(headers, host);
	RequestDisplay();
	// printf("><<><><><%d %d\n", error, status);
	// exit(0);
	// files, plus check the syntaxe
}
/*end*/