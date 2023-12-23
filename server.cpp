#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <vector>
#define TMP_DOMAIN		AF_INET
#define TYPE		SOCK_STREAM
#define PROTOCOL	0// this is related to the protocol that support thr TYPE in our normal case SOCK_STREAM
#define BACKLOG		1
#define DEFAUL_THEADER_RESPONSE  "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello world!"
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
		ServerId = socket(TMP_DOMAIN, TYPE, PROTOCOL);
		if (ServerId == -1)
			return (std::cerr << "Error : Fail to Creat the Socket" << std::endl, 1);
		std::cout << RED << "------>Socket is created (id == " << ServerId << ")."  << COLOR_END << std::endl;
		/*Bind our Socket with a name(adresse)*/
		address.sin_family = TMP_DOMAIN;
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
		char	buff[4046];
		std::vector<int> clients;
		while (true)
		{
			int CSocket = accept(ServerId, (sockaddr*)&address, &addlen);
			if (CSocket == -1)
				return (std::cerr << "Error : Fail to Accept Client connection" << std::endl, 1);
			std::cout << RED << "------>Waiting ... " << COLOR_END << std::endl;
			std::cout << RED << "------>Connect to Client " << CSocket << "." << COLOR_END << std::endl;
			/*Read In Comming that*/
			bzero(buff, 4046);
			std::cout << RED << "Resived message : " << std::endl;
			read(CSocket, &buff, 4046);
			
			// gave the client its proper response
			std::cout << buff << std::endl;
			std::cout << "Send the Response to the client\n" << std::endl;
			std::string response = DEFAUL_THEADER_RESPONSE;
			std::cout << response << std::endl;
			write(CSocket, response.c_str(), response.length());
			std::cout << COLOR_END << "\nEnded" << std::endl;
		}
	}
	else
		std::cout << "The sever will need only the Port id!!!" << std::endl;
	return(0);
}