/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_listening.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/14 13:26:32 by adardour          #+#    #+#             */
/*   Updated: 2024/03/19 01:58:47 by aalami           ###   ########.fr       */
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
	size_t i = 0;
	std::string str;
	std::string port;
	std::string host;
	std::string ip_address;
	int find;
	while (i < serverBlocks.getDirectives().size())
	{
		if (!serverBlocks.getDirectives()[i].getDirective().compare("listen"))
		{
			str = serverBlocks.getDirectives()[i].getArgument()[0];
			find = str.find(':');
			if (find == -1)
			{
				host = "0.0.0.0";
				port = str.substr(find + 1);
			}
			else if (find == 0)
			{
				printf("not valid!");
				exit(1);
			}
			else
			{
				host = str.substr(0,find);
				port = str.substr(find + 1);
			}
			if ((atoi(port.c_str()) < 0 || atoi(port.c_str()) > 65535) || !valid_port(port))
			{
				printf("not valid!");
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
		std::memset(&hints,0,sizeof(hints));
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
			std::memset(&port_host,0,sizeof(port_host));
			matched_server_block.insert(std::make_pair(socket_fd,i));
		}
		freeaddrinfo(result);
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

void	ShowLogs(Client &client)
{
 	std::time_t now = std::time(NULL);
    
    std::tm *time_info = std::localtime(&now);
	
	std::string method =  client.getHttp_request().getMethod();
	std::string path = client.getHttp_request().getUri().path;

	std::cout << " " << (time_info->tm_year + 1900) << '/' << (time_info->tm_mon + 1) << '/' << time_info->tm_mday << " ";
    std::cout << " " << time_info->tm_hour << ':' << time_info->tm_min << ':' << time_info->tm_sec << " ";
	std::cout << "[REQUEST]  " ;
	printf("%s ",method.c_str());
	printf("/%s\n",path.c_str());
	((request &)client.getHttp_request()).setDisplay(true);
}

void    handle_request(std::vector<struct pollfd> &poll_fds, int i, std::vector<ServerBlocks> &serverBlocks, Client & client, std::map<int, int> &matched_server_block)
{
	ssize_t bytes_read;
		char buffer[CHUNK_SIZE + 1];
	if (!client.getHttp_request().getRequestRead())
	{
		bytes_read = read(poll_fds[i].fd, buffer, CHUNK_SIZE);
		if (bytes_read > 0)
			client.BufferingRequest(serverBlocks, buffer, matched_server_block, bytes_read);
		if (bytes_read < 0)
		{
			((request &)client.getHttp_request()).setError(true);
			((request &)client.getHttp_request()).setStatus(500);
			((request &)client.getHttp_request()).setRequestRead(true);
		}
		if (bytes_read > 0)
			buffer[bytes_read] = '\0';
	}
	// if (client.getHttp_request().getStartLine() && !client.getHttp_request().getDisplay())
	// 	ShowLogs(client);
	if (client.getHttp_request().getRequestRead())
	{
		client.ParseRequest();
		((request &)client.getHttp_request()).setHandleRequest(true);
	}
}

bool	RangeFormat(const HTTPHeader &header, response  &response, long long FileSize)
{
	long long	start = 0;
	long long	end = -1;
	
	std::string A,S,E; 
	
	if (header.values.size() > 0)
	{
		std::string value = header.values;
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

std::string	HandleHeaderFileStatus(Client& client, long long size)
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
		if (!RangeFormat(header, (response  &)resp, size))
		{
			((response  &)resp).setFileIndex(0),((response  &)resp).setFileEnd(size);
		}
		size_t s = resp.getFileIndex();
		long long e = resp.getFileEnd() - 1;
		std::string start = ToString(s), end = ToString(e);
		ResponseHeader += "Content-Range: bytes "+ start +"-"+ end +"/"+ FileSize + "\r\n";
	}
	if (index == -1)
		((response  &)resp).setFileIndex(0), ((response  &)resp).setFileEnd(size);
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
		if (resp.getHeader_size() == (long long)resp.getHeader_index())
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
			if (resp.getBody_size() == (long long)resp.getBody_index())
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
		std::string ExtratHeader = HandleHeaderFileStatus(client, size);
		buffer += ExtratHeader;
		if (resp.getBody_size() == (long long)resp.getBody_index())
			resp.setBody_sent(true);
	}
}

void	FileSeeking(response &resp, request & req, int fd)
{
	size_t 	size_buff = 1000000;
	char	*buff = (char *)std::malloc(sizeof(char) * 1000000);
	if (!buff)
	{
		resp.setBody_sent(true);
		resp.setHeader_sent(true);
		req.setError(true);
		return ;
	}
	ssize_t bytes_read;
	size_t fileIndex = resp.getFileIndex();
	if (size_buff != 0 && resp.getSeeker() != fileIndex)
	{
		if (fileIndex - resp.getSeeker() < size_buff)
			size_buff = fileIndex - resp.getSeeker();
		bytes_read = read(fd, buff, size_buff);
		if (bytes_read > 0)
			resp.setSeeker(resp.getSeeker() + bytes_read);
		if (bytes_read < 0)
		{
			resp.setBody_sent(true);
			resp.setHeader_sent(true);
			req.setError(true);
		}
	}
	if (resp.getSeeker() == fileIndex)
		resp.setFileSeeked(true);
	std::free(buff), buff = NULL;
}

void	getFilePartionBuffer(response &resp, request &req,int fd, std::string &buffer, size_t &writeBytes)
{
	char buff[writeBytes];
	if (resp.getFileEnd() - resp.getFileIndex() < writeBytes)
		writeBytes = (resp.getFileEnd() - resp.getFileIndex());
	ssize_t bytes = read(fd, buff, writeBytes);
	if (bytes > 0)
	{
		for (ssize_t i = 0; i < bytes; i++)
			buffer += buff[i];
		size_t Bindex = resp.getBody_index();
		size_t Findex = resp.getFileIndex();

		resp.setBody_index(Bindex + bytes);
		resp.setFileIndex(Findex + bytes);
	}
	else if (bytes == -1)
	{
		resp.setBody_sent(true);
		resp.setHeader_sent(true);
		req.setError(true);
	}
}

void	BodyFileResponse(response &resp, Client& client, std::string &file, std::string &buffer, size_t &writeBytes)
{
	if (resp.getFileOpened() == false)
		AdditionnalHeaders(client, resp, file, buffer);
	if (resp.getBody_sent() == false && resp.getFileEnd() > 0 && writeBytes > 0)
	{
		int fd = resp.getFd();
		if (fd == -1)
		{
			fd = open(file.c_str(), O_RDONLY);
			if (fd == -1)
			{
				resp.setBody_sent(true);
				resp.setHeader_sent(true);
				((request &)client.getHttp_request()).setError(true);
				return ;
			}
			resp.setFd(fd), resp.setFileOpened(true);
		}
		if (fd > 0)
		{
			request &req = (request &)client.getHttp_request();
			if (!resp.getFileSeeked())
				FileSeeking(resp, req,fd);
			if (resp.getFileSeeked())
				getFilePartionBuffer(resp, req, fd, buffer, writeBytes);
		}
	}
	if (resp.getFileEnd() == resp.getFileIndex() || resp.getFileEnd() <= 0 || resp.getFileEnd() < resp.getFileIndex())
	{
		resp.setBody_sent(true);
		if (resp.getFd() != -1)
			close(resp.getFd()), resp.setFd(-1);
	}
}

void	handle_response(std::vector<struct pollfd> &poll_fds,int i, Client & client, std::map<unsigned int, std::string> &status_codes)
{
	std::string buffer;
	response &resp = (response &)client.getHttp_response();;
	size_t writeBytes = CHUNK_SIZE;

	client.CreateResponse(status_codes);
	if (resp.getReadyToResponed())
	{
		if (resp.getFile() == "")
		{
			HeadersTransfert(resp, writeBytes, buffer);
			BodyStringBodyTransfert(resp, writeBytes, buffer);
			if (resp.getBody_sent() == false && resp.getBody_size() <= 0)
				resp.setBody_sent(true);
		}
		else
		{
			if (resp.getHeader_sent() == false)
				HeadersTransfert(resp, writeBytes, buffer);
			if (resp.getBody_sent() == false && writeBytes > 0)
			{
				std::string file = resp.getFile();
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
		}
	}
}

int    new_connection(int client_socket,std::vector<int> new_connections)
{
	for (size_t i = 0; i < new_connections.size(); i++)
	{
		if (new_connections[i] == client_socket)
			return (0);
	}
	return (1);
}


size_t findClientBySocketFd(std::vector<Client> &ClientsVector, int fd)
{
	size_t i = 0;
	
	for (i = 0 ; i <  ClientsVector.size(); i++)
	{
		if (ClientsVector[i].getClientSocket() == fd)
			break;
	}
	return i;
}
void handleCgiResponse(Client & client, std::map<unsigned int, std::string> &status_codes)
{
	CgiResponse &resp = client.getcgiResponse();

	if (!resp.isResponseSent())
	{
		if (!resp.isReqObjectset())
		{
			CgiEnv &req = client.getcgiRequest();
			resp.setCgiEnvObject(req);
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
	std::vector<int> vecFds;
	std::vector<struct pollfd> poll_fds;
	struct pollfd tmp; 
	int pollRet = 0;
	int acceptRet = 0;
	std::string human_status;
	std::string mime_type;
	std::map<int, int> matched_server_block;

	create_sockets(serverBlocks, sockets,matched_server_block);
	init_poll_fds(poll_fds, serverBlocks.size(), sockets);
	std::vector<int> new_connections;
	std::vector<Client> ClientsVector;
	
	std::cout<<GREEN<<"Waiting for an incoming request... "<<RESET<<std::endl;
	while (true)
	{
		pollRet = poll(poll_fds.data(), poll_fds.size(), TIME_OUT);
		if (pollRet < 0)
		{
			perror("poll ");
			exit(0);
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
				if (acceptRet < 0)
				{
				    std::cerr<<"Can't Connect to the client"<<std::endl;
					perror("Accept : ");
					continue;
				}
				client.setFdServer(poll_fds[i].fd);
				client.setClientSocket(acceptRet);
				ClientsVector.push_back(client);
				if (fcntl(acceptRet, F_SETFL,  O_NONBLOCK , FD_CLOEXEC) < 0)
				{   perror("fctnl");
					continue;
				}
				std::memset(&tmp, 0, sizeof(tmp));
				tmp.fd = acceptRet;
				tmp.events = POLLIN;
				poll_fds.push_back(tmp); 
			}
			else if (i >= sockets.size())
			{
				size_t client_it;
				client_it = findClientBySocketFd(ClientsVector, poll_fds[i].fd);
				if (client_it == ClientsVector.size())
					continue;
				if (poll_fds[i].revents & POLLIN)
				{
					handle_request(poll_fds,i,serverBlocks, ClientsVector[client_it],matched_server_block);
					if (ClientsVector[client_it].getHttp_request().getHandleRequest())
						poll_fds[i].events = POLLOUT;
					if ( ClientsVector[client_it].getInProcess() == false 
						&& CurrentTime() - ClientsVector[client_it].getTime() > C_TIMEOUT)
					{
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
					
					if (ClientsVector[client_it].get_cgi_status() && !ClientsVector[client_it].getcgiResponse().isResponseSent() )
						handleCgiResponse(ClientsVector[client_it], status_codes);
					else
						handle_response(poll_fds,i, ClientsVector[client_it], status_codes);
					if ((ClientsVector[client_it].getHttp_response().getBody_sent() && ClientsVector[client_it].getHttp_response().getHeader_sent()) || ClientsVector[client_it].getcgiResponse().isResponseSent())
					{
						if(!isAlive(ClientsVector[client_it]) || ClientsVector[client_it].getHttp_request().getError() || ClientsVector[client_it].getcgiResponse().isError())
						{
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
							ClientsVector.erase(ClientsVector.begin() + client_it);
							ClientsVector.insert(ClientsVector.begin() + client_it, client);
							poll_fds[i].events = POLLIN;
						}
					}
				}
				else if (poll_fds[i].revents & POLLHUP)
				{
					if (ClientsVector[client_it].getHttp_response().getFd() != -1)
						close(ClientsVector[client_it].getHttp_response().getFd());
					ClientsVector.erase(ClientsVector.begin() + client_it);
					close(poll_fds[i].fd);
					poll_fds.erase(poll_fds.begin() + i);
					i--;
				}
				else
				{
					if (ClientsVector[client_it].getInProcess() == false  && CurrentTime() - ClientsVector[client_it].getTime() > C_TIMEOUT)
					{
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