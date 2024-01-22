
#include "WorkerInit.hpp"

void    setAllowedmethods2(Worker &worker, std::vector<Directives> &directives)
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

void    setErrorPages2(Worker &worker, std::vector<Directives> &directives)
{
    for (size_t i = 0; i < directives.size(); i++)
    {
        if (!directives[i].getDirective().compare("error_page"))
        {
            worker.setErrorPages(directives[i].getArgument());
            return;
        }
    }
}

void    build_response2(Worker &worker,std::string &response)
{
    std::ifstream myfile;
    myfile.open(worker.getIndex());
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
    else {
        std::cout << "Couldn't open file\n";
    }
}

std::string    is_mime_type2(std::string &path)
{
    int length = path.length();
    while (length)
    {
        if (path[length] == '.')
        {
            break;
        }
        length--;
    }
    return (path.substr(length + 1));
}

void    response_mime_type2(std::string &response, std::string &mime_type, int *status)
{
    int length = response.length();
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/" + mime_type + "\nContent-Length: " + std::to_string(length) + "\n\n" + response;
    *status = 1;
}

bool is_directive2(const std::string &directive)
{
    std::vector<std::string> tokenTypes;
    
    tokenTypes.push_back("client_max_body_size");
    tokenTypes.push_back("error_page");
    tokenTypes.push_back("autoindex");
    tokenTypes.push_back("allow_methods");
    tokenTypes.push_back("index");
    tokenTypes.push_back("access_log");
    tokenTypes.push_back("error_log");
    tokenTypes.push_back("root");
    tokenTypes.push_back("to");
    tokenTypes.push_back("cgi");
    
    if (std::find(tokenTypes.begin(), tokenTypes.end(), directive) != tokenTypes.end())
    {
        return true;
    }
    return false;
}

void    WorkerInit(Worker &worker, std::vector<ServerBlocks> &serverBlocks, std::string &path, std::string &host)
{
    worker = Worker(serverBlocks, host);
    worker.setLocationWorker(worker.getBlockWorker(),path);
    set2(worker.getLocationWorker().getDirectives(),worker);
    if (worker.getRoot().empty())
    {
        set2(worker.getBlockWorker().getDirectives(),worker);
    }
    setDirectives2(worker.getBlockWorker().getDirectives(), worker, 0);//why 
    setDirectives2(worker.getLocationWorker().getDirectives(), worker, 1);// why
    setAllowedmethods2(worker,worker.getLocationWorker().getDirectives());
    if (worker.getAllowMethods().size() == 0)
        setAllowedmethods2(worker, worker.getBlockWorker().getDirectives());
    setErrorPages2(worker, worker.getLocationWorker().getDirectives());
    if (worker.getErrorPages().size() == 0)
        setErrorPages2(worker, worker.getBlockWorker().getDirectives());
	// std::cout << " ROOT4 " << worker.getRoot()<< std::endl;
    // if (!worker.getRoot().empty())
    //     worker.setRoot(worker.getRoot() + path);
	// std::cout << " ROOT5 " << worker.getRoot()<< std::endl;
}