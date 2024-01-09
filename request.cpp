#include "request.hpp"

request::request()
{

}

// using this fucntion we can avoid adding the SP and HT plus LWS to our request string and replace them all bye a space
void replaceConsecutiveSpaces(std::string &HTTPrequest, std::string &req)
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
		std::cout << HTTPrequest << "\n\n i == " << i << std::endl;
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
		std::cout << "pos--> " << pos << '\n';
		index = pos;
	}
	else
		line = str.substr(index + s), index = std::string::npos;
	std::cout << index << '\n';
	return (line);
}

void	ParseHeaders(std::vector<HTTPHeader> &headers, std::string &request, size_t &index, bool &error, int &status)
{
	std::string KnownHeaders[] = {"Host", "Accept", "Accept-Language", "Accept-Encoding", "Connection", "Referer"};
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
			tmp = spliter(request, delimiter, index);
			if (tmp.size() == 0)
				continue;;
			std::cout << "tmp :: " << tmp << std::endl;
			// surpace space if it was at first i think this was allow
			if (tmp[i] == ' ')
				i++;
			while (tmp[i] && tmp[i] != ' ' && tmp[i] != ':')
				header.name += tmp[i++];
			if (tmp[i] == ' ')
				i++;
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
			stop = true;
	}
}

void	MethodParsing(bool &error, int &status, std::string &HTTPrequest, std::string &method, std::string &uri, std::string &http, size_t &index)
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
		{
			error = true,status = 400;// is there another way to say undefine ? 
		}
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
	if (tmp[i] == ' ')
		i++;
	while (tmp[i] && tmp[i] != ' ')
		http += tmp[i++];

	// check the http protocol
	if (http != "HTTP/1.1")
		error = true,status = 400;
}

request::request(char *r) : req(r)
{
	std::string HTTPrequest;
	HTTPHeader HTTPHeaders;
	error = false;
	status = 200;
	size_t index = 0;
	// Turn any none quoted  *SP or HT or LWS to one space
	std::cout << "Before Request form :\n" << req <<std::endl;
	replaceConsecutiveSpaces(HTTPrequest, req);
	// Define the method  ?
	MethodParsing(error, status, HTTPrequest, method, uri, http, index);
	// parsthe headers
	ParseHeaders(headers, req, index, error, status);
	std::cout << "Request :\n" << HTTPrequest <<std::endl;
	std::cout << "methode : " << method << ", uri " << uri << ", http protocol : " << http << " error " << error << " status " << status <<std::endl;
	for (std::vector<HTTPHeader>::iterator iter = headers.begin();  iter != headers.end(); iter++)
	{
		std::cout <<"--> "<< iter->name << " : ";
		for(std::list<std::string>::iterator it = iter->values.begin(); it != iter->values.end(); it++)
		{
			std::cout << *it << " ";
		}
		std::cout << std::endl;
	}
	// files, plus check the syntaxe
	
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
