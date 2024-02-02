#include "http.server.hpp"


void	stringStreamRest(std::stringstream &ss)
{
	ss.str("");
	ss.clear();
}


long long ft_atoll(const char* str)
{
	size_t		i = 0;
	bool		Psign = true;
	long long	result = 0;
	// surpace all the space

	while (str[i] == ' ')
		i++;
	if (str[i] == '+')
		i++;
	else if (str[i] == '-')
	{
		Psign = false;
		i++;
	}
	while (std::isdigit(str[i]))
	{
		result = (result * 10) + str[i] - '0';
		i++;
	}
	return Psign ? result : -result;
}

std::string     NormilisePath(std::string &Path)
{
	std::string result;
	bool slash = false;
	for (size_t i = 0; i < Path.size(); i++)
	{
		if (Path[i] == '/' && slash == false)
		{
			result += '/';
			slash = true;
		}
		else if (Path[i] != '/')
		{
			result += Path[i];
			slash = false;
		}
	}
	return (result);
}
