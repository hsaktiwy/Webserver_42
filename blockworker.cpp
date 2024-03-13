/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blockworker.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 15:21:48 by adardour          #+#    #+#             */
/*   Updated: 2024/03/13 01:29:33 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"


Worker::Worker()
{
	this->track_status = 0;
	this->isCgi = false;
}

ServerBlocks Worker::getBlockWorker() const
{
	return this->blockworker;
}

void Worker::setBlockWorker(const ServerBlocks& blocks)
{
	this->blockworker = blocks;
}

LocationsBlock Worker::getLocationWorker() const
{
	return this->locationworker;
}

std::string Worker::getRoot() const
{
	return this->root;
}

void Worker::setRoot(const std::string& newRoot)
{
	this->root = newRoot;
}
std::string Worker::getIndex() const
{
	return this->index;
}

std::string Worker::getRedirect() const
{
	return redirect;
}

void Worker::setRedirect(const std::string& newRedirect)
{
		redirect = newRedirect;
}


std::vector<std::string> const &Worker::getAllowMethods() const  
{
	return this->allow_methods;
}
	

std::string trim(const std::string& str)
{
	size_t start = str.find_first_not_of(" \t\n\r\f\v");

	if (start == std::string::npos) 
		return "";

	size_t end = str.find_last_not_of(" \t\n\r\f\v");
	return str.substr(start, end - start + 1);
}

std::string ft_trim(const std::string& str, const std::string& targets)
{
	size_t start = str.find_first_not_of(targets);

	if (start == std::string::npos) 
		return "";

	size_t end = str.find_last_not_of(targets);
	return str.substr(start, end - start + 1);
}

Worker::Worker(std::vector<ServerBlocks> &blocks,std::string &host)
{    
	for (size_t i = 0; i < blocks.size(); i++)
	{
		for (size_t j = 0; j < blocks[i].getDirectives().size(); j++)
		{
			if (!blocks[i].getDirectives()[j].getDirective().compare("listen"))
			{
				for (size_t k = 0; k < blocks[i].getDirectives()[j].getArgument().size(); k++)
				{
					if (!trim(blocks[i].getDirectives()[j].getArgument()[0]).compare(trim(host)))
					{
						setBlockWorker(blocks[i]);
						return;
					}
				}
			}
		}
	}
}

bool prefix(const std::string &prefix, const std::string &path)
{   
	if (prefix.compare("/"))
	{
    	return path.find(prefix) == 0;
	}
	return false;
}
bool Worker::exact_match(const ServerBlocks &block,std::string &path)
{
	if (path[0] == '/')
	{
		path.erase(0, 1);
	}
	for (size_t i = 0; i < block.getLocations().size(); i++)
	{
		std::string location_prefix = block.getLocations()[i].getPath();
		if (location_prefix[0] == '/')
			location_prefix.erase(0,1);
		if (!location_prefix.compare(path))
		{
			locationworker = block.getLocations()[i];
			return true;
		}
	}
	return false;
}
bool Worker::prefix_match(const ServerBlocks &block,std::string &path)
{
	path = "/" + path;
	for (size_t i = 0; i < block.getLocations().size(); i++)
	{
		std::string location_prefix = block.getLocations()[i].getPath();
		if (prefix(location_prefix,path))
		{
			this->locationworker = block.getLocations()[i];
			return true;
		}
	}
	return false;
}

bool Worker::find_root(const ServerBlocks &block)
{
	for (size_t i = 0; i < block.getLocations().size(); i++)
	{
		if (block.getLocations()[i].getPath().compare("/") == 0)
		{
			this->locationworker = block.getLocations()[i];
			return true;
		}
	}
	return false;
}

void    Worker::setLocationWorker(const ServerBlocks& block,std::string &path)
{
	if (exact_match(block,path))
		return;
	else if (prefix_match(block,path))
		return ;
	else if (find_root(block))
		return ;
}

void Worker::setIndex(const std::vector<std::string>&   args,const std::string &root)//, const std::string &path)
{
	for (size_t i = 0; i < args.size(); i++)
	{
		if (access(((root + path ) + args[i]).c_str(),F_OK) == 0)
		{
			this->index = args[i];
			break;
		}
	}
}

std::vector<std::vector<std::string> >  &Worker::getErrorPages()   
{
	return this->error_page;
}

std::string Worker::getHost() const
{
		return host;
}

void Worker::setHost(const std::string& newHost)
{
	host = newHost;
}

std::string Worker::getPath() const {
	return path;
}

void Worker::setPath(const std::string& newPath)
{
	path = newPath;
}

std::string Worker::getAutoIndex() const {
	return this->autoindex;
}

void Worker::setAutoIndex(const std::string& newPath)
{
	this->autoindex = newPath;
}
std::string &Worker::get_max_body_size() 
{
	return (this->max_body_size);
}

void Worker::set_max_body_size(std::string max_body_size) 
{
	this->max_body_size = max_body_size;
}

void Worker::setMethod(std::vector<std::string>  &args)
{
	for (size_t i = 0; i < args.size(); i++)
	{
		allow_methods.push_back(args[i]);
	}
}

void    Worker::setErrorPages(std::vector<std::string>  &args)
{
	std::vector<std::string> error_page;
	for (size_t i = 0; i < args.size(); i++)
	{
		error_page.push_back(args[i]);
	}
	this->getErrorPages().push_back(error_page);
	error_page.clear();
}

void    Worker::found_index_file(const std::string &root)
{
	DIR* dir = opendir(root.c_str());
	if (dir)
	{
		struct dirent* start;
		while ((start = readdir(dir)) != NULL)
		{
			if (!strcmp(start->d_name,"index.html") || !strcmp(start->d_name,"index.htm"))
			{
				if (Is_Directory(this->getRoot() + "/" + start->d_name) == 1)
				{        
					this->index = start->d_name;
					break;
				}
			}
		}
		closedir(dir);
	}
}

std::string    Worker::getPathError() const 
{
	return this->path_error_page;
}

void    Worker::setPathError(const std::vector<std::vector<std::string> > &error_page,  int status)
{
	this->set_track_status(0);
	for (size_t i = 0; i < error_page.size(); i++)
	{
	   for (size_t j = 0; j < error_page[i].size() - 1; j++)
	   {
			if (atoi(error_page[i][j].c_str()) == status)
			{
				this->set_track_status(1);
				this->path_error_page = error_page[i][error_page[i].size() - 1];
			}
	   }
	}    
}

void Worker::set_track_status(int flag)
{
	this->track_status = flag;
}
int Worker::get_track_status()
{
	return this->track_status;
}
void Worker::setQuery(const std::string &query)
{
	this->query = query; 
}
std::string Worker::getQuery() const
{
	return(this->query);
}

void Worker::setCgiStatus(bool state)
{
	this->isCgi = state;
}
bool Worker::getCgiStatus()
{
	return isCgi;
}

void    Worker::setPathUpload(std::string const &path)
{
	this->path_upload = path;
}
std::string    &Worker::getPathUpload()
{
	return this->path_upload;
}

std::string Worker::get_bash_bin() const
{
	return this->cgi_bash;
}

std::string Worker::get_python_bin() const
{
	return this->cgi_python;
}

void	Worker::set_bash_bin(const std::string &path)
{
	this->cgi_bash = path;
}
void	Worker::set_python_bin(const std::string &path)
{
	this->cgi_python = path;
}