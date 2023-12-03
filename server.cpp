#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>

#define DOMAIN		AF_INET
#define TYPE		SOCK_STREAM
#define PROTOCOL	0// this is related to the protocol that support thr TYPE in our normal case SOCK_STREAM
#define BACKLOG		1

#define RED "\033[34m"
#define RED2 "\033[45m"
#define COLOR_END "\033[00m"

bool	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (true);
	return (false);
}

bool	isNumber(char *arg)
{
	for(int i = 0; arg[i] ; i++)
	{
		if (!ft_isdigit(arg[i]))
			return (false);
	}
	return (true);
}

int main(int argc, char **argv)
{
	struct sockaddr_in address;
	int ServerId;
	int PortId;

	if (argc == 2)
	{
		/*Check for if the Port input is valide*/
		if (!isNumber(argv[1]))
			return (std::cerr << "Error : Invalude Port!!!" << std::endl, 1);
		/*Converte the Port input (char *) to int value*/
		PortId = std::atoi(argv[1]);
		/*Create SOCKET*/
		ServerId = socket(DOMAIN, TYPE, PROTOCOL);
		if (ServerId == -1)
			return (std::cerr << "Error : Fail to Creat the Socket" << std::endl, 1);
		std::cout << RED << "------>Socket is created (id == " << ServerId << ")."  << COLOR_END << std::endl;
		/*Bind our Socket with a name(adresse)*/
		address.sin_family = DOMAIN;
		address.sin_port = htons(PortId);
		address.sin_addr.s_addr = INADDR_ANY;
		bzero(address.sin_zero, sizeof(address.sin_zero));
		int binded = bind(ServerId, (const sockaddr*)&address, sizeof(address));
		if (binded == -1)
			return (std::cerr << "Error : Fail to Bind Socket" << std::endl, 1);
		std::cout << RED << "------>Socket adress is Binded."  << COLOR_END << std::endl;
		/*listening */
		int listening = listen(ServerId, BACKLOG);
		if (listening == -1)
			return (std::cerr << "Error : Fail in listening" << std::endl, 1);
		std::cout << RED << "------>We are listing."  << COLOR_END << std::endl;
		/*Accepting connection*/
		socklen_t addlen = sizeof(address);
		while (true)
		{
			std::cout << RED << "------>Waiting ... " << COLOR_END << std::endl;
			int CSocket = accept(ServerId, (sockaddr*)&address, &addlen);
			if (CSocket == -1)
				return (std::cerr << "Error : Fail to Accept Client connection" << std::endl, 1);
			std::cout << RED << "------>Connect to Client " << CSocket << "." << COLOR_END << std::endl;
			/*Read In Comming that*/
			char	buff;
			std::cout << RED << "Resived message : " << std::endl;
			while (read(CSocket, &buff, 1))
				write(1, &buff, 1);
			std::cout << COLOR_END << "\nEnded" << std::endl;
			close(CSocket);
		}
	}
	else
		std::cout << "The sever will need only the Port id!!!" << std::endl;
	return(0);
}