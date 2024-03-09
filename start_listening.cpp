/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_listening.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/14 13:26:32 by adardour          #+#    #+#             */
/*   Updated: 2024/03/09 04:36:18 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"
#include "Client.hpp"
#include <map>
#include <unistd.h>
#include "cgi/cgi.hpp"
#include "cgi/cgiResponse.hpp"

bool valid_port(const std::string &port)
{
	int i = 0;
	while (port[i] != '\0')
	{
		if (!isdigit(port[i]))
			return false;
		i++;
	}

	return true;
}
void    get_port_host(ServerBlocks &serverBlocks,t_port_host &port_host)
{
	int i = 0;
	std::string str;
	std::string port;
	std::string host;
	std::string ip_address;
	int find = 0;
	while (i < serverBlocks.getDirectives().size())
	{
		if (!serverBlocks.getDirectives()[i].getDirective().compare("listen"))
		{
			str = serverBlocks.getDirectives()[i].getArgument()[0];
			int find = str.find(':');
			port = str.substr(find + 1);
			host = str.substr(0,find);
			if ((atoi(port.c_str()) < 0 || atoi(port.c_str()) > 65535) || !valid_port(port))
			{
				printf("error \n");
				exit(1);
			}
			port_host.port = atoi(port.c_str());
			port_host.host = host;
			break;
		}
		i++;
	}
}

void    create_sockets(std::vector<ServerBlocks> &serverBlocks,std::vector<int> &sockets,std::map<int, int> &matched_server_block)
{
	int socket_fd;
	struct addrinfo *result,*p,hints;
	t_port_host port_host;
	int status;
	for (size_t i = 0; i < serverBlocks.size(); i++)
	{
		ft_memset(&hints,0,sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		get_port_host(serverBlocks[i],port_host);
		if ((status = getaddrinfo(port_host.host.c_str(),std::to_string(port_host.port).c_str(), &hints, &result)) != 0)
		{
			std::cerr << "error: " << gai_strerror(status) << std::endl;
			exit(0);
		}
		for (p = result; p != NULL; p = p->ai_next)
		{
			socket_fd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
			if (socket_fd == -1)
			{
				perror("socket");
				exit(1);
			}
			if(fcntl(socket_fd,F_SETFL, O_NONBLOCK | O_CLOEXEC) < 0)
			{
				perror("set socket ");
				exit(1);
			}
			int opt = 1;
			if (setsockopt(socket_fd, SOL_SOCKET,  SO_REUSEPORT, &opt, sizeof(opt)) == -1)
			{
				perror("set sockopt ");
				exit(1);
									
			}
			if(bind(socket_fd,p->ai_addr,p->ai_addrlen) < 0)
			{
				perror("bind socket ");
				exit(1);
									
			}
			if (listen(socket_fd, 0) < 0)
			{
				perror("socket ");
				exit(1);
									
			}
			sockets.push_back(socket_fd);
			ft_memset(&port_host,0,sizeof(port_host));
			matched_server_block.insert(std::make_pair(socket_fd,i));
		}
	}
}

void    init_poll_fds(std::vector<struct pollfd> &poll_fds,int size,std::vector<int> &sockets)
{
	for (int i = 0; i < size; i++)
	{
		struct pollfd temp;
		temp.fd = sockets[i];
		temp.events = POLLIN;
		poll_fds.push_back(temp);
	}
}

int create_socket_client(std::vector<int> &sockets,std::vector<struct pollfd> &poll_fds, nfds_t *size_fd, int i)
{
	int client_socket;
	sockaddr_in client;
	socklen_t socklen = sizeof(client);
	client_socket = accept(sockets[i], (struct sockaddr *)&client, &socklen);
	if (client_socket < 0)
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
		{
			return 35;
		}
		else
		{
			std::cout << "Error in accepting connection\n";
			return -1;
		}
	}
	else
	{
		struct pollfd temp;
		temp.fd = client_socket;
		temp.events = POLLIN | POLLOUT; 
		poll_fds.push_back(temp);
		(*size_fd)++;
	}
	return (client_socket);
}

// void handle_read(std::vector<struct pollfd> &poll_fds, int i, int *ready_to_write, 
//                   nfds_t *size_fd, std::vector<ServerBlocks> &serverBlocks, std::string &response, int *flag,int *status,std::string &human_status,std::string &mime_type)
// {
//     char buffer[1024];
//     int bytes_read = recv(poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
//     if (bytes_read > 0)
//     {
//         buffer[bytes_read] = '\0';
//         init_worker_block(buffer, serverBlocks);
//         *ready_to_write = 1;
//     }
//     else if (bytes_read == 0)
//     {
//         printf("close client\n");
//         close(poll_fds[i].fd);
//         poll_fds.erase(poll_fds.begin() + i);
//         (*size_fd)--;
//     }
//     else
//     {
//         perror("recv ");
//     }
// }

void	ShowLogs(char (&buffer)[CHUNK_SIZE + 1])
{
 	std::time_t now = std::time(NULL);
    
    std::tm *time_info = std::localtime(&now);
	
	std::string method;
	std::string path;
	
	std::stringstream object(buffer);
	std::string firstLine;

	
	object >> method >> path;
	
	std::cout << " " << (time_info->tm_year + 1900) << '/' << (time_info->tm_mon + 1) << '/' << time_info->tm_mday << " ";
    std::cout << " " << time_info->tm_hour << ':' << time_info->tm_min << ':' << time_info->tm_sec << " ";
	std::cout << "[REQUEST]  " ;
	printf("%s ",method.c_str());
	printf("%s ",path.c_str());
	printf("\n");
}

void    handle_request(std::vector<struct pollfd> &poll_fds, int i,int *ready_to_write, nfds_t *size_fd,std::vector<ServerBlocks> &serverBlocks,std::string &response, Client & client, std::map<unsigned int, std::string> &status_codes,std::map<int, int> &matched_server_block)
{
	ssize_t bytes_read, bytes_toread = CHUNK_SIZE;
	static size_t file_size;
	// this part where we will read chunk off our request from the client socket,
	// and parse it at the same time to define if it ended or not
	if (!client.getHttp_request().getRequestRead())
	{
		char buffer[CHUNK_SIZE + 1];
		bytes_read = read(poll_fds[i].fd,buffer, CHUNK_SIZE);
		if (bytes_read > 0)
			client.BufferingRequest(serverBlocks,buffer,matched_server_block,bytes_read);
		if (bytes_read < 0)
		{
			((request &)client.getHttp_request()).setError(true);
			((request &)client.getHttp_request()).setStatus(500);
			((request &)client.getHttp_request()).setRequestRead(true);
		}
		buffer[bytes_read] = '\0';
		// if(std::strlen(buffer) > 0)
		// {
		// 	ShowLogs(buffer);
			
		// }
		// std::cout << RED << "BYTES Read From the request " << bytes_read << RESET<< std::endl;
	}
	// this part where we will handle some additional request parsing, at the time where the request was fully read
	if (client.getHttp_request().getRequestRead())
	{
		// printf("Body :\n%s\n", ((request &)client.getHttp_request()).getBody().c_str());
		// exit(0);
		client.ParseRequest(serverBlocks);
		((request &)client.getHttp_request()).setHandleRequest(true);
	}
}

bool	RangeFormat(const HTTPHeader &header, response  &response, long long FileSize)
{
	bool Valide = true;
	long long	start = 0;
	long long	end = -1;
	std::string A,S,E; 
	if (header.values.size() > 0)
	{
		std::string value = header.values;
		// Range: bytes=0-1023 bytes=S-|| A=S-E
		int i = 0;
		while (value[i] && std::isalpha(value[i]))
			A += value[i++];
		if (value[i] && value[i] != '=' && A != "bytes")
			return (false);
		i++;
		while (value[i] && std::isdigit(value[i]))
			S+=value[i++];
		if (value[i] && value[i] != '-')
			return (false);
		i++;
		while (value[i] && std::isdigit(value[i]))
			E+=value[i++];
		if (value[i])
			return (false);
		// check and initial the responce index and the repsonce end;
		if (S != "")
			start = ft_atoll(S.c_str());
		if (E != "")
			end = ft_atoll(E.c_str()) + 1;
		else
			end = FileSize;
		if (end < start || start < 0 || end < 0 || end > FileSize || start > FileSize)
			return (false);
		response.setFileIndex(start);
		response.setFileEnd(end);
		return (true);
	}
	return (false);
}

// LMTime mean the last 'file modification time'
std::string	HandleHeaderFileStatus(Client& client, long long size, long long LMTime)
{
	const request   				&request = client.getHttp_request();
	std::string 					ResponseHeader;
	const response  				&resp = client.getHttp_response();
	const std::vector<HTTPHeader>	&headers = request.getHeaders();
	int             				index;
	std::string 					FileSize = ToString(size);

	index  = request.getHeaderIndex("Range");
	if (index != -1)
	{
		const HTTPHeader &header = headers[index];
		// check the value format
		if (!RangeFormat(header, (response  &)resp, size))
		{
			// default form start  = 0 to end  == file size
			((response  &)resp).setFileIndex(0),((response  &)resp).setFileEnd(size);
		}
		size_t s = resp.getFileIndex();
		long long e = resp.getFileEnd() - 1;
		std::string start = ToString(s), end = ToString(e);
		ResponseHeader += "Content-Range: bytes "+ start +"-"+ end +"/"+ FileSize + "\r\n";// + Etag;
	}
	// Define if possible to add Accept-Range
	if (index == -1)
		((response  &)resp).setFileIndex(0), ((response  &)resp).setFileEnd(size);
	// Define the Content-Lenght
	size_t s = resp.getFileIndex();
	long long e = resp.getFileEnd(), rest = (e - s);
	ResponseHeader += "Content-Length: " + ToString(rest) + "\r\n";
	if (index == -1)
	{
		std::string FileType = resp.getFileType();
		if (FileType.find("video") != std::string::npos
			|| FileType.find("audio") != std::string::npos)
			ResponseHeader += "Accept-Ranges: bytes\r\n"; 
	}
	ResponseHeader += "Server: " + ((std::string)SERVERNAME) + "\r\n\r\n"; 
	((response  &)resp).setBody_size(rest + ResponseHeader.size());
	return (ResponseHeader);
}

void	HeadersTransfert(response &resp, size_t &writeBytes, std::string &buffer)
{
	long long Hsize = resp.getHeader_size();
	long long Hindex = resp.getHeader_index();
	if (Hindex < Hsize)
	{
		size_t bytes = writeBytes;
		long long rest = Hsize - Hindex;
		if (rest <  CHUNK_SIZE)
			bytes = rest;
		buffer = resp.getHttp_response().substr(Hindex, bytes);
		resp.setHeader_index(Hindex + bytes);
		if (resp.getHeader_size() == resp.getHeader_index())
			resp.setHeader_sent(true);
		writeBytes -= bytes;
	}
}

void	BodyStringBodyTransfert(response &resp, size_t &writeBytes, std::string &buffer)
{
	if (resp.getBody_sent() == false && writeBytes > 0)
	{
		long long Bsize = resp.getBody_size();
		long long Bindex = resp.getBody_index();

		if (Bindex < Bsize)
		{
			size_t bytes = writeBytes;
			long long rest = Bsize - Bindex;
			if (rest <  CHUNK_SIZE)
				bytes = rest;
			buffer += resp.getBody_string().substr(Bindex, bytes);
			resp.setBody_index(Bindex + bytes);
			if (resp.getBody_size() == resp.getBody_index())
				resp.setBody_sent(true);
			writeBytes -= bytes; 
		}
	}
}

void	AdditionnalHeaders(Client &client, response &resp, std::string &file, std::string &buffer)
{
	struct stat stat_buff;
	int error = stat(file.c_str(), &stat_buff);
	if (error == 0)
	{
		long long size = stat_buff.st_size;
		long LMTime = stat_buff.st_mtimespec.tv_sec;
		std::string ExtratHeader = HandleHeaderFileStatus(client, size, LMTime);
		buffer += ExtratHeader;
		if (resp.getBody_size() == resp.getBody_index())
			resp.setBody_sent(true);
	}
}

void	FileSeeking(response &resp, int fd)
{
	size_t 	size_buff = 1000000;
	char	*buff = (char *)malloc(sizeof(char) * 1000000);
	if (!buff)
		size_buff = 0; 
	ssize_t bytes_read;
	size_t fileIndex = resp.getFileIndex();
	if (size_buff != 0 && resp.getSeeker() != fileIndex)
	{
		if (fileIndex - resp.getSeeker() >= size_buff)
			size_buff = size_buff;
		else
			size_buff = fileIndex - resp.getSeeker();
		bytes_read = read(fd, buff, size_buff);
		if (bytes_read > 0)
			resp.setSeeker(resp.getSeeker() + bytes_read);
	}
	if (resp.getSeeker() == fileIndex)
		resp.setFileSeeked(true);
	free(buff), buff = NULL;
}

void	getFilePartionBuffer(response &resp, int fd, std::string &buffer, size_t &writeBytes)
{
	char buff[writeBytes];
	if (resp.getFileEnd() - resp.getFileIndex() < writeBytes)
		writeBytes = (resp.getFileEnd() - resp.getFileIndex());
	// printf("rest Off the File %lu(File Index = %lu, File End = %lu) writeBytes Reading Size %lu", (resp.getFileEnd() - resp.getFileIndex()) , resp.getFileIndex(), resp.getFileEnd(), writeBytes);
	// printf("	%lu\n", writeBytes);
	ssize_t bytes = read(fd, buff, writeBytes);
	if (bytes > 0)
	{
		for (size_t i = 0; i < bytes; i++)
			buffer += buff[i];
		size_t Bindex = resp.getBody_index();
		size_t Findex = resp.getFileIndex();

		resp.setBody_index(Bindex + bytes);
		resp.setFileIndex(Findex + bytes);
	}
	else if (bytes == -1)
		write(2, "Something Went Wrong!\n", 22), exit(0);
}

void	BodyFileResponse(response &resp, Client& client, std::string &file, std::string &buffer, size_t &writeBytes)
{
	if (resp.getFileOpened() == false)
		AdditionnalHeaders(client, resp, file, buffer);
	// static int NOpen;
	// SEND BODY FILE
	if (resp.getBody_sent() == false && resp.getFileEnd() > 0 && writeBytes > 0)
	{
		int fd = resp.getFd();
		// INITIALIZE FILE DISCRIPTOR
		if (fd == -1)
		{
			fd = open(file.c_str(), O_RDONLY);
			if (fd == -1)
				perror("Open :");
			resp.setFd(fd), resp.setFileOpened(true);
		}
		// SEND BODY FILE: IN THE FORM OFF CHUNKS MAX SIZE == CHUNK_SIZE
		// IN RANGE CASE GO TO FILEINDEX POSITION
		if (fd > 0)
		{
			if (!resp.getFileSeeked())
				FileSeeking(resp, fd);
			if (resp.getFileSeeked())
				getFilePartionBuffer(resp, fd, buffer, writeBytes);
		}
	}
	// the case where thw file has no size i mean 0
	if (resp.getFileEnd() == resp.getFileIndex() || resp.getFileEnd() <= 0 || resp.getFileEnd() < resp.getFileIndex())
	{
		resp.setBody_sent(true);
		if (resp.getFd() != -1)
			close(resp.getFd()), resp.setFd(-1);
	}
}

void	handle_response(std::vector<struct pollfd> &poll_fds,int i,int *ready_to_write, nfds_t *size_fd,std::string &string_response,int *flag,int *status,std::string &human_status,std::string &mime_type, Client & client, std::map<unsigned int, std::string> &status_codes)
{
	std::string buffer;// this will hold our chunked response
	response &resp = (response &)client.getHttp_response();
	Worker &wk = (Worker&)client.getWorker();
	size_t writeBytes = CHUNK_SIZE;

	client.CreateResponse(status_codes);
	if (resp.getReadyToResponed())
	{
		if (resp.getFile() == "")
		{
			// BODY STRING CASE : where i our code creat the body and it don't need a file at all, this scope will handle it
			// SEND Headers
			HeadersTransfert(resp, writeBytes, buffer);
			// SEND Body
			BodyStringBodyTransfert(resp, writeBytes, buffer);
			if (resp.getBody_sent() == false && resp.getBody_size() <= 0)
			{
				resp.setBody_sent(true);
			}
		}
		else
		{
			// BODY FILE : when our body is in file format
			// SEND Headers
			if (resp.getHeader_sent() == false)
				HeadersTransfert(resp, writeBytes, buffer);
			// SEND BODY Plus SOME HEADERS
			if (resp.getBody_sent() == false && writeBytes > 0)
			{
				std::string file = resp.getFile();
				// this part of code will try to send a additional headers at once, this wont effect our preformance
				BodyFileResponse(resp, client, file, buffer, writeBytes);
			}
		}
		if (buffer.size() > 0)
		{
			ssize_t bytes_written = send(poll_fds[i].fd, buffer.c_str(), buffer.size(), MSG_DONTWAIT);
			if (bytes_written < 0)
			{
				resp.setBody_sent(true);
				resp.setHeader_sent(true);
				((request &)client.getHttp_request()).setError(true);
			}
			// here we need to chekc if we have 0 or -1 as result
			// printf("bytes rigth %lu\n",bytes_written);
		}
	}
}
// here we will write our response in all cases in our client socket
int    new_connection(int client_socket,std::vector<int> new_connections)
{
	for (size_t i = 0; i < new_connections.size(); i++)
	{
		if (new_connections[i] == client_socket)
			return (0);
	}
	return (1);
}

// void start_listening_and_accept_request(std::vector<ServerBlocks> &serverBlocks, std::map<unsigned int, std::string> &status_codes)
// {
//     std::string response;
//     std::vector<int> sockets;
//     std::vector<struct pollfd> poll_fds;
//     char buffer[1024];
//     int client_socket = -1;
//     int ready_to_write = 0;
//     int flag = 0;
//     int status;
//     std::string human_status;
//     std::string mime_type;

//     create_sockets(serverBlocks, sockets);
//     init_poll_fds(poll_fds, serverBlocks.size(), sockets);
//     std::vector<int> new_connections;
//     std::vector<Client> clients;
//     nfds_t size_fd = poll_fds.size();
//     while (true)
//     {
//         int ready = poll(poll_fds.data(), size_fd, -1);
//         if (ready < 0)
//         {
//             perror("poll ");
//             exit(0);
//         }
//         for (size_t i = 0; i < size_fd; i++)
//         {
//             if ((poll_fds[i].revents & POLLIN))
//             {
//                 if (poll_fds[i].fd == sockets[i])
//                 {
//                     client_socket = create_socket_client(sockets, poll_fds, &size_fd,i);
					
//                     if (client_socket == 35 || client_socket == -1)
//                     {
//                         if (client_socket == 35)
//                             continue;
//                         else
//                         {
//                             std::cout << "Error in accepting connection\n";
//                             break;
//                         }
//                     }
//                 }
//                 else
//                 {
//                     Client client;
//                     if (poll_fds[i].revents & POLLIN)
//                     {
//                         if(new_connection(client_socket,new_connections))
//                         {
//                             printf("new connection from socket %d ...\n",client_socket);
//                         }
//                         //handle_read(poll_fds, i, &ready_to_write, &size_fd,serverBlocks, response, &flag,&status,human_status, http_request);
//                         handle_request(poll_fds,i,&ready_to_write,&size_fd,serverBlocks, response, client, status_codes);
//                         // CgiEnv cgi(client.getWorker());
//                         // cgi.setCgiServerName();
//                         // cgi.setCgiServePort();
//                         // cgi.setCgiScriptPath();
//                         // std::cout<<"SERVER_NAME   : "<<cgi.getCgiServerName()<<" SERVER_PORT :  "<<cgi.getCgiServerPort()<<std::endl;
//                         //handle_read(poll_fds, i, &ready_to_write, &size_fd,serverBlocks, response, &flag,&status,human_status,mime_type);
//                     }
//                     if (poll_fds[i].revents & POLLOUT)
//                     {
//                         handle_response(poll_fds,i,&ready_to_write, &size_fd,response, &flag,&status,human_status,mime_type, client, status_codes);
//                     }
//                 }
//             }
//         }
//     }
// }

size_t findClientBySocketFd(std::vector<Client> &ClientsVector, int fd)
{
	size_t i = 0;
	
	for (i = 0 ; i <  ClientsVector.size(); i++)
	{
		// printf("Socket id  : %d\n", it->getClientSocket());
		if (ClientsVector[i].getClientSocket() == fd)
			break;
	}
	return i;
}
void handleCgiResponse(std::vector<struct pollfd> &poll_fds, size_t i, Client & client, std::map<unsigned int, std::string> &status_codes)
{
	CgiResponse &resp = client.getcgiResponse();

	if (!resp.isResponseSent())
	{
		if (!resp.isReqObjectset())
		{	resp.setCgiEnvObject(client.getcgiRequest());
			resp.setSocket(client.getClientSocket());
			resp.setErrorMap(status_codes);
		}
		if(!resp.isEnvset())
		{
			resp.constructScriptEnv();
		}
		resp.creatCgiResponse();
	}
}

bool	isAlive(Client & client)
{
	const request &req = client.getHttp_request();
	int index = req.getHeaderIndex("Connection");
	if (index  == -1)
		return true;
	if (index != -1)
	{
		std::vector<HTTPHeader> &headers = (std::vector<HTTPHeader> &)req.getHeaders();
		size_t pos = headers[index].values.find("keep-alive");
		if (pos != std::string::npos)
			return (true);
	}
	return (false);
}

void start_listening_and_accept_request(std::vector<ServerBlocks> &serverBlocks,std::map<unsigned int, std::string> &status_codes)
{
	std::string response;
	std::vector<int> sockets;
	// std::vector<t_socket_data> socketsFds;
	std::vector<int> vecFds; // save only opened sockets on a vector (useful when working with poll)
	std::vector<struct pollfd> poll_fds;
	struct pollfd tmp; 
	// int timeout = 2 * 60 * 1000;
	int pollRet = 0;
	int acceptRet = 0;
	char buffer[1024];
	int client_socket = -1;
	int ready_to_write = 0;
	int flag = 0;
	int status;
	std::string human_status;
	std::string mime_type;
	std::map<int, int> matched_server_block;

	create_sockets(serverBlocks, sockets,matched_server_block);
	init_poll_fds(poll_fds, serverBlocks.size(), sockets);
	std::vector<int> new_connections;
	std::vector<Client> ClientsVector;
	
	nfds_t size_fd = poll_fds.size();
	std::cout<<GREEN<<" Waiting for an incomming request "<<RESET<<std::endl;
	
	// std::map<int, int>::iterator it = matched_server_block.begin();
	// std::map<int, int>::iterator ite = matched_server_block.end();

	// while (it != ite)
	// {
	// 	printf("%d %d\n",it->first,it->second);
	// 	it++;
	// }
	
	// exit(0);
	while (true)
	{
		// std::cerr << "Polling " << std::endl;
		pollRet = poll(poll_fds.data(), poll_fds.size(), TIME_OUT);
		if (pollRet < 0)
		{
			perror("poll ");
			exit(0);
		}
		if (pollRet == 0)
		{
			std::cout<<RED<<"Connection timeout...."<<RESET<<std::endl;
			continue;
		}
		for (size_t i = 0; i < poll_fds.size(); i++)
		{
			if (i < sockets.size() && (poll_fds[i].revents & POLLIN))
			{
				Client client;
				socklen_t len;
				struct sockaddr_in tmpAddr;
				len = sizeof(tmpAddr);
				
				acceptRet = accept(poll_fds[i].fd, (struct sockaddr *)&tmpAddr, &len);
				// if (acceptRet < 0)
				//     break;
				client.setFdServer(poll_fds[i].fd);
				client.setClientSocket(acceptRet);
				// printf("lol %lld %lu\n", client.getHttp_response().getHeader_size(),client.getHttp_response().getHeader_index());
				ClientsVector.push_back(client);
				char clientIP[INET_ADDRSTRLEN];
				unsigned short clientPort = ntohs(tmpAddr.sin_port);
				inet_ntop(AF_INET, &(tmpAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
				// std::cout<<GREEN <<"New incomming connection from the client Socket ";
				// std::cout<<acceptRet<<" with the address : "<<clientIP<<":"<<clientPort<< " To the socket "<<poll_fds[i].fd<<RESET<<std::endl;
				if (fcntl(acceptRet, F_SETFL,  O_NONBLOCK , FD_CLOEXEC) < 0)
				{   perror("fctnl");
					continue;
				}
				ft_memset(&tmp, 0, sizeof(tmp));
				tmp.fd = acceptRet; // add a client socket to the poll array
				tmp.events = POLLIN; // waiting for I/O on this socket
				poll_fds.push_back(tmp); 
					// pollFdsSize++;
			}
			else if (i >= sockets.size())
			{
				// printf("Client Number : %lu\n", ClientsVector.size() );
				size_t client_it;
				client_it = findClientBySocketFd(ClientsVector, poll_fds[i].fd);
				if (client_it == ClientsVector.size())
				{
					printf("%lu, %lu, %d, %lu\n", client_it, ClientsVector.size(), poll_fds[i].fd, i);
					printf("OUT\n");
					continue ;
				}
				if (poll_fds[i].revents & POLLIN)
				{
					// printf("%sClient info [%lu][%d] client time here from %ld%s\n", YELLOW, client_it, ClientsVector[client_it].getClientSocket(),  ClientsVector[client_it].getTime(), RESET);
					handle_request(poll_fds,i,&ready_to_write,&size_fd,serverBlocks, response, ClientsVector[client_it], status_codes,matched_server_block);
					if (ClientsVector[client_it].getHttp_request().getHandleRequest())
					{
						poll_fds[i].events = POLLOUT;
					}
					if ( ClientsVector[client_it].getInProcess() == false 
						&& CurrentTime() - ClientsVector[client_it].getTime() > C_TIMEOUT)
					{
						std::cout<<RED<<"Too long time waiting for data  Client "<<poll_fds[i].fd<<" closed the connection"<<RESET<<std::endl;
						if (ClientsVector[client_it].getHttp_response().getFd() != -1)
							close(ClientsVector[client_it].getHttp_response().getFd());
						ClientsVector.erase(ClientsVector.begin() + client_it);
						close(poll_fds[i].fd);
						poll_fds.erase(poll_fds.begin() + i);
						i--;
					}
				}
				else if (poll_fds[i].revents & POLLOUT)
				{
					if (ClientsVector[client_it].getHttp_request().getCgiStatus() && !ClientsVector[client_it].getcgiResponse().isResponseSent())
						handleCgiResponse(poll_fds, i, ClientsVector[client_it], status_codes);
					else
						{handle_response(poll_fds,i,&ready_to_write, &size_fd,response, &flag,&status,human_status,mime_type, ClientsVector[client_it], status_codes);
						// std::cout<<BLUE<<"Part Of Response sent to: " <<poll_fds[i].fd<<" !! [ availble Clients " << ClientsVector.size() << ", Client index " << client_it << "]"<<RESET<<std::endl;
						}
					// std::cout<<BLUE<<"Part Of Response sent to: " <<poll_fds[i].fd<<" !! [ availble Clients " << ClientsVector.size() << ", Client index " << client_it << "]"<<RESET<<std::endl;
					if (ClientsVector[client_it].getHttp_response().getBody_sent() && ClientsVector[client_it].getHttp_response().getHeader_sent())
					{
						
						if(!isAlive(ClientsVector[client_it]) || ClientsVector[client_it].getHttp_request().getError() || ClientsVector[client_it].getcgiResponse().isError())
						{
							// printf("%d _ %d\n", isAlive(ClientsVector[client_it]), ClientsVector[client_it].getHttp_request().getError());
							// std::cout<<BLUE<<"Response sent to: " <<poll_fds[i].fd<<" !!"<<RESET<<std::endl;
							// std::cout<<YELLOW<<"Connection to Client "<<poll_fds[i].fd<<" closed"<<RESET<<std::endl;
							ClientsVector.erase(ClientsVector.begin() + client_it);
							close(poll_fds[i].fd);
							poll_fds.erase(poll_fds.begin() + i);
							i--;
						}
						else
						{
							Client client;
							client.setClientSocket(ClientsVector[client_it].getClientSocket());
							client.setFdServer(ClientsVector[client_it].getFdServer());
							// ClientsVector[client_it] = client;
							ClientsVector.erase(ClientsVector.begin() + client_it);
							ClientsVector.insert(ClientsVector.begin() + client_it, client);
							poll_fds[i].events = POLLIN;
							// initialize all request and response values
						}
					}
				}
				else if (poll_fds[i].revents & POLLHUP)
				{
					std::cout<<RED<<"Client "<<poll_fds[i].fd<<" closed the connection"<<RESET<<std::endl;
					if (ClientsVector[client_it].getHttp_response().getFd() != -1)
						close(ClientsVector[client_it].getHttp_response().getFd());
					ClientsVector.erase(ClientsVector.begin() + client_it);
					close(poll_fds[i].fd);
					poll_fds.erase(poll_fds.begin() + i);
					i--;
				}
				else
				{
					if (ClientsVector[client_it].getInProcess() == false && CurrentTime() - ClientsVector[client_it].getTime() > C_TIMEOUT)
					{
						// std::cout<<RED<<"Unknown Client "<<poll_fds[i].fd<<" closed the connection"<<RESET<<std::endl;
						if (ClientsVector[client_it].getHttp_response().getFd() != -1)
							close(ClientsVector[client_it].getHttp_response().getFd());
						ClientsVector.erase(ClientsVector.begin() + client_it);
						close(poll_fds[i].fd);
						poll_fds.erase(poll_fds.begin() + i);
						i--;
					}
				}
			}
		}
	}
}