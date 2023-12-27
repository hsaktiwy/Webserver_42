#include "request.hpp"

request::request()
{

}

// using this fucntion we can avoid adding the SP and HT plus LWS to our request string and replace them all bye a space
void replaceConsecutiveSpaces(std::string HTTPrequest, std::string &req)
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

std::string spliter(std::string &str, const std::string & delimiter, size_t &index)
{
	size_t s = ((index == 0) ? 0 : delimiter.size());
	size_t pos = str.find(delimiter, (index + s < str.size()) ? (index + s) : str.size() - 1);
	std::string line;

	if (pos != std::string::npos)
	{
		line = str.substr(index + s, pos);
		index = pos;
	}
	else
		line = str.substr(index + s);
	return (line);
}

void	ParseHTTPRequest(std::vector<HTTPHeader> &headers, std::string &request, std::string &Headers)
{
	std::string tmp;
	std::string delimiter = "\r\n";
	size_t 		index = request.find(delimiter);

	if (index != std::string::npos)
	{
		// get all the headers
		tmp = spliter(request, delimiter, index);
	}// else in case where there is no delimiter at all
}

request::request(char *r) : req(r)
{
	std::string HTTPrequest;
	HTTPHeader HTTPHeaders;
	error = false;
	status = 200;
	std::set<std::string> ValideMethodes = {"GET", "POST", "DELETE"};
	std::string KnownHeaders[] = {"Host", "Accept", "Accept-Language", "Accept-Encoding", "Connection", "Referer"};
	// Turn any none quoted  *SP or HT or LWS to one space
	replaceConsecutiveSpaces(HTTPrequest, req);
	// Define the methode merge all the duplicate header file ? this i don't new if i need to handle it
	size_t index = 0;
	// Define the methode
	std::string tmp = spliter(HTTPrequest, "\r\n", index);
	method = "";
	size_t i = 0;
	if (tmp[0] && !std::isalpha(tmp[0]))
	{
		error = true;
		status = 400; //  bad request
	}
	// get the methode
	while (tmp[i] && error == false)
	{
		if (std::isalpha(tmp[i]))
			method += tmp[i];
		else
		{
			error = true;
			status = 400; //  bad request
		}
	}
	// check if the header file is valide
	if (error == false)
	{
		std::set<std::string>::iterator iter = ValideMethodes.find(method);
		if (iter == ValideMethodes.end())
		{
			error = true;
			status = 400;
		}
	}
	// get the uri
	uri = "";
	while (tmp[i] && error == false)
	{
		
		if (tmp[i] != ' ')
	}
	// files, plus check the syntaxe
	
}


request::~request()
{
 
}

request::request(const request& copy)
{

}

request& request::operator=(const request& obj)
{
	if (this != &obj)
	{

	}
	return (*this);
}
