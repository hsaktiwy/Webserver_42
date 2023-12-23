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

std::string spliter(std::string &str, std::string & delimiter, size_t index)
{
	size_t pos = str.find(delimiter, (index + delimiter.size() < str.size()) ? (index + delimiter.size()) : str.size() - 1);
	std::string line;

	if (pos != std::string::npos)

}

void	ParseHTTPRequest(HTTPHeader &holder, std::string &request)
{
	std::string tmp;
	std::string delimiter = "\r\t";
	size_t 		index = 0;
	
}

request::request(char *r) : req(r)
{
	std::string HTTPrequest;
	HTTPHeader HTTPHeaders;

	// Turn any none quoted  *SP or HT or LWS to one space
	replaceConsecutiveSpaces(HTTPrequest, req);
	// Define the methode merge all the duplicate header file
	
	// Define the header files, plus check the syntaxe
	
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
