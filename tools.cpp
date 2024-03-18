#include "http.server.hpp"



long long ft_atoll(const char* str)
{
	size_t		i = 0;
	bool		Psign = true;
	long long	result = 0;

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

void	ExtractValues(std::string &str, std::string  &holder, size_t &index)
{
	char c = '\0';
	while (str[index])
	{
		if (str[index] == '\"' && c != '\"')
			c = '\"';
		else if ((str[index] == '\"' && c == '\"') || (str[index] == ' ' && c == '\0'))
			break;
		else
			holder += str[index];
		index++;
	}
}