/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_worker_block.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 19:33:06 by adardour          #+#    #+#             */
/*   Updated: 2024/03/20 03:49:44 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/http.server.hpp"

void    setAllowedmethods(Worker &worker, std::vector<Directives> &directives)
{
    for (size_t i = 0; i < directives.size(); i++)
    {
        if (!directives[i].getDirective().compare("allow_methods"))
        {
            worker.setMethod(directives[i].getArgument());
            return;
        }
    }
}

void    setErrorPages(Worker &worker, std::vector<Directives> &directives)
{
	for (size_t i = 0; i < directives.size(); i++)
	{
		if (!directives[i].getDirective().compare("error_page"))
		{
			worker.setErrorPages(directives[i].getArgument());
		}
	}
}

template<typename T>
void    set(T& Directives,Worker &worker)
{
	for (size_t i = 0; i < Directives.size(); i++)
	{
		if (!Directives[i].getDirective().compare("root"))
		{
			worker.setRoot(Directives[i].getArgument()[0]);
			break;
		}
	}
}

template<typename T>
void    setDirectives(T &directives,Worker &worker)
{
	for (size_t i = 0; i < directives.size(); i++)
	{
		if (!directives[i].getDirective().compare("client_max_body_size"))
		{
			worker.set_max_body_size(directives[i].getArgument()[0]);
		}
		else if (!directives[i].getDirective().compare("autoindex"))
		{
			worker.setAutoIndex(directives[i].getArgument()[0]);
		}
		else if (!directives[i].getDirective().compare("to"))
		{
			worker.setRedirect(directives[i].getArgument()[0]);
		}
		else if (!directives[i].getDirective().compare("index"))
		{
			worker.setIndex(directives[i].getArgument(),worker.getRoot());
		}
		else if (!directives[i].getDirective().compare("uploads"))
		{
			worker.setPathUpload(directives[i].getArgument()[0]);
		}
		else if (!directives[i].getDirective().compare("cgi_python"))
		{
			worker.set_python_bin(directives[i].getArgument()[0]);
		}
		else if (!directives[i].getDirective().compare("cgi_bash"))
		{
			worker.set_bash_bin(directives[i].getArgument()[0]);
		}
	}
    
}

int Is_Directory(const std::string &root)
{
	struct stat fileInfo;

	if (stat(root.c_str(),&fileInfo) == 0)
	{
		if (S_ISDIR(fileInfo.st_mode))
		{
			return (0);
		}
		else if (S_ISREG(fileInfo.st_mode))
		{
			return (1);
		}
	}
	return (-1);
}

void    get_port(std::vector<Directives> &directives,std::string &port) 
{
    size_t i = 0;
    while (i < directives.size())
    {
        if (!directives[i].getDirective().compare("listen"))
        {
            port = directives[i].getArgument()[0].substr(directives[i].getArgument()[0].find(':') + 1);
            break;
        }
        i++;
    }
    
}
int match_by_host(std::string &hostname,std::vector<ServerBlocks> &blocks,Worker &worker)
{
    const size_t size_blocks = blocks.size();
    for (size_t i = 0; i < size_blocks; i++)
    {
        const size_t size_directives = blocks[i].getDirectives().size();
        for (size_t j = 0; j <  size_directives; j++)
        {
            if (!blocks[i].getDirectives()[j].getDirective().compare("listen"))
            {
                std::string listen = blocks[i].getDirectives()[j].getArgument()[0];
                if (!listen.compare(hostname))
                {
                    worker.setBlockWorker(blocks[i]);
                    return 1;
                }
            }
        }
    }
    return 0;
}
void    get_matched_server_block(std::string &host_name,std::vector<ServerBlocks> &blocks,Worker &worker,int fd_server,std::map<int, int> &matched_server_block)
{
    const size_t size_blocks = blocks.size();
    std::string host = host_name.substr(0,host_name.find(':'));
    for (size_t i = 0; i < size_blocks; i++)
    {
        const size_t size_directives = blocks[i].getDirectives().size();
        for (size_t j = 0; j <  size_directives; j++)
        {
            if (!blocks[i].getDirectives()[j].getDirective().compare("server_names"))
            {
                const size_t size_arguments = blocks[i].getDirectives()[j].getArgument().size();
                for (size_t k = 0; k < size_arguments; k++)
                {
                    if (!blocks[i].getDirectives()[j].getArgument()[k].compare(host))
                    {
                        std::string port;
                        get_port(blocks[i].getDirectives(),port);
                        if (!port.compare(host_name.substr(host_name.find(':') + 1)))
                        {   
                            worker.setBlockWorker(blocks[i]);
                            return;
                        }
                        port.clear();
                    }
                }
            }
        }
    }
    if (match_by_host(host_name,blocks,worker))
        return;
    std::map<int, int>::iterator it = matched_server_block.begin();
    std::map<int, int>::iterator ite = matched_server_block.end();
    while (it != ite)
    {
        if (it->first == fd_server)
        {
            worker.setBlockWorker(blocks[it->second]);
            return;
        }
        it++;
    }
}


void   init_worker_block(Worker &worker, std::string &host ,std::string &path,std::vector<ServerBlocks> &serverBlocks, int &is_dir, int &is_regular,int fd,std::map<int, int> &matched_server_block)
{
    std::string port;

    std::string host_name;
    worker.setPath(path);
    get_matched_server_block(host,serverBlocks,worker,fd,matched_server_block);

    worker.setLocationWorker(worker.getBlockWorker(),path);
    set(worker.getLocationWorker().getDirectives(),worker);
    if (worker.getRoot().empty())
        set(worker.getBlockWorker().getDirectives(),worker);
    setDirectives(worker.getBlockWorker().getDirectives(),worker);
    setDirectives(worker.getLocationWorker().getDirectives(),worker);
    setAllowedmethods(worker,worker.getLocationWorker().getDirectives());

    if (worker.getAllowMethods().size() == 0)
        setAllowedmethods(worker,worker.getBlockWorker().getDirectives());
        
    setErrorPages(worker,worker.getLocationWorker().getDirectives());
    if (worker.getErrorPages().size() == 0)
        setErrorPages(worker,worker.getBlockWorker().getDirectives());
    std::string tmp_path = worker.getRoot() + path;
    if (Is_Directory(tmp_path) == 0 \
    || Is_Directory(tmp_path) == 1 \
    || Is_Directory(tmp_path) == -1)
    {
        if (Is_Directory(tmp_path) == 0)
        {
            if (worker.getIndex().empty())
                worker.found_index_file(tmp_path);
            is_dir = 1;
        }
        else if (Is_Directory(tmp_path) == 1)
            is_regular = 1;
    }
    if (worker.getPathUpload().empty())
    {
        worker.setPathUpload("Uploads/");
    }
}