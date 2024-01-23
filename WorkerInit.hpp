#ifndef WORKERINIT_HPP
#define WORKERINIT_HPP

#include "http.server.hpp"
template<typename T>
void    set2(T& Directives,Worker &worker)
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
void    setDirectives2(T &directives,Worker &worker,int flag)
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
    }
    
}

void    WorkerInit(Worker &worker, std::vector<ServerBlocks> &serverBlocks, std::string &path, std::string &host);

#endif