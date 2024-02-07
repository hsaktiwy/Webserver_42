/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_serving.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaktiwy <hsaktiwy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 14:43:51 by adardour          #+#    #+#             */
/*   Updated: 2024/02/03 16:56:53 by hsaktiwy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

void    read_content(const std::string &root,std::string &response,std::string &type,std::string &mime_type)
{
	if (type.compare("css"))
	{
		mime_type = "text/css";
	}
	else
	{
		mime_type = "text/html";
	}
	std::ifstream myfile;
	myfile.open(root);
	std::string myline;
	if ( myfile.is_open() )
	{
		while ( myfile )
		{
			std::getline (myfile, myline);
			response += myline;
		}
		myfile.close();
	}
	else
	{
		std::cout << "Couldn't open file\n";
	}
}

std::string	get_mimetype(const std::string &root)
{
	int length = root.size();
	while (length)
	{
		if (root[length] == '.')
		{
			break;
		}
		length--;
	}
	
	return root.substr(length + 1).c_str();
}

void start_serving(const Worker &worker,std::string &response,std::string &human_status,int *status,std::string &mime_type)
{   
	std::string type;
	if (!worker.getIndex().empty())
	{
		type = get_mimetype(worker.getRoot());
		read_content(worker.getIndex(),response,type,mime_type);
	}
	else
	{
		printf("not found\n");   
		*status = 404;
		human_status = "Not Found";
		response += "<h1>Not found</h1>";
	}
}