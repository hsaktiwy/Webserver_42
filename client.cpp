#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <fcntl.h>

#define MY_DOMAIN		AF_INET
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
	int clientId;
	int PortId;

	if (argc == 3)
	{
		/*Check for if the Port input is valide*/
		if (!isNumber(argv[1]))
			return (std::cerr << "Error : Invalude Port!!!" << std::endl, 1);
		/*Converte the Port input (char *) to int value*/
		PortId = std::atoi(argv[1]);
		/*Create SOCKET*/
		clientId = socket(MY_DOMAIN, TYPE, PROTOCOL);
		if (clientId == -1)
			return (std::cerr << "Error : Fail to Creat the Socket" << std::endl, 1);
		std::cout << RED << "------>Socket is created (id == " << clientId << ")."  << COLOR_END << std::endl;
		/*Connecting */
		address.sin_addr.s_addr = INADDR_ANY;//gethostname(argv[1], strlen(argv[1]));
		address.sin_port = htons(PortId);
		address.sin_family = MY_DOMAIN;
		bzero(address.sin_zero, sizeof(address.sin_zero));
		char buff[4400];
			int conneted = connect(clientId, (const sockaddr*) &address, sizeof(address));
			if (conneted == -1)
				return(std::cerr << "Error : Fail to establish Connect width the host localhost in port " << PortId << std::endl, 1);
			std::cout << RED << "------>Connection established width the localhost in port "<< PortId << "."  << COLOR_END << std::endl;
		fcntl(clientId, F_SETFL, O_NONBLOCK);
		while (true)
		{
			std::string message, tmp;
			std::cout << "Prepare your message on the " << argv[2] << " file and click on enter : ";
			std::getline(std::cin, tmp);
			std::ifstream file(argv[2]);
			if (file.is_open())
			{
				std::cout << "File is opened\n";
				while (std::getline(file, tmp))
				{
					// std::cout << "geting line\n";
					message += tmp;
					message += "\r\n";
				}
				std::cout << "end\n";
			}
			else
				std::cerr  << "Error :\n	Couldn't ope the file there is problem fix it! waiting for a hint? no i want gave it to you!" << std::endl;
			file.close();
			std::cout << "\nMessage : \n" << message << "\n\n" << std::endl;
			// if (message == "exit")
			// 	return (close(clientId));
			write(clientId, message.c_str(), message.length());// we may think to change write with send.
			write(clientId, "\r\n", 2);
			std::cout << RED << "------> responce : "  << COLOR_END << std::endl;
			std::ofstream outfile("outputResponce.txt");
			bzero(buff, 4400);
			if (outfile.is_open())
			{
				std::cout << "Output is in the outputResponce.txt file:";
				ssize_t bit = recv(clientId, &buff, 4400, MSG_DONTWAIT);
				if (bit ==  -1)
					std::cerr << "Fail to recv!" << std::endl;
				else
					outfile << buff;
				outfile.close();
			}
			else
			{
				std::cout << "Unable to open the file, so let display on the console (stdout): \n";
				ssize_t bit = recv(clientId, &buff, 4400, MSG_DONTWAIT);
				if (bit ==  -1)
					std::cerr << "Fail to recv!" << std::endl;
				else
					std::cout << buff;
				std::cout << buff << std::endl;
			}
			std::cout << std::endl;
		}
	}
	else
		std::cout << "The sever will need hostname followed by Port id +  request input file!!!" << std::endl;
	return(0);
}