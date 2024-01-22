#include "responce.hpp"

responce::responce(request &req, Worker &wk): http_request(&req), worker(&wk)
{
    
}

void    responce::responed(std::map<unsigned int, std::string> &status_codes)
{
    std::string index = (*worker).getIndex();
    request &req = *http_request;
    Worker &wk = *worker;

    if (req.getError() == false)
    {
        if (req.getIs_dir() == 1 || req.getIs_regular() == 1)
        {
            if (req.getIs_dir() == 1 && index.size() == 0 &&  wk.getAutoIndex() == "on")
            {
                // autoindexing
            }
            else if (req.getIs_dir() == 1 && index.size() == 0)
            {
                req.setError(true), req.setStatus(404);
            }
        }
        else
        {
            req.setError(true), req.setStatus(404);
            // responed using error pages
            
        }
    }
    // responed using error pages if  (error  == true)
    if (req.getError() == true)
    {
        // responce for Error handling
        errorResponce(status_codes);
    }
    else
        http_responce = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html>\r\n<head>\r\n	<title>Valide File</title>\r\n</head>\r\n<body>\r\n	<h1>The responce must be here!.</h1>\r\n</body>\r\n</html>\r\n";
}

void    responce::errorResponce(std::map<unsigned int, std::string> &status_codes)
{
    Worker &wk = *worker;
    request &req = *http_request;
    std::string html_body;
    std::string HumanRead;
    std::stringstream ss,ss2;
    std::string statusCode;
    std::vector<std::string> error_pages = wk.getErrorPages();
    std::map<unsigned int, std::string>::iterator iter = status_codes.find(req.getStatus());
    ss << req.getStatus();
    ss >> statusCode;
    if (iter != status_codes.end())
        HumanRead = iter->second;
    printf("||||||||||||%s\n", error_pages[2].c_str());
    // if (error_pages.size() == 0)
    // {
        html_body = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Error Page</title>\r\n<style>\r\nbody {\r\nfont-family: Arial, sans-serif;\r\ntext-align: center;\r\npadding-top: 50px;\r\n}\r\nh1 {\r\nfont-size: 3em;\r\ncolor: #990000;\r\nmargin-bottom: 20px;\r\n}\r\np {\r\nfont-size: 1.5em;\r\ncolor: #666666;\r\nmargin-bottom: 50px;\r\n}\r\n</style>\r\n</head>\r\n<body>\r\n<h1>Error "+ statusCode + "("+ HumanRead +")"+"</h1>\r\n<p>Unhable to reserve a propore responce.</p>\r\n</body>\r\n</html>";
        http_responce += "HTTP/1.1 " + statusCode + " " + HumanRead + "\r\n";
        http_responce += "Content-Type: text/html\r\n";
        std::string size;
        ss2 << html_body.size();
        ss2 >> size;
        http_responce += "Content-Length: " + size + "\r\n\r\n" + html_body;
    // }
    // else
    // {
    //     // this will be handled when the error_page directive will be ready
    //     http_responce = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html>\r\n<head>\r\n	<title>Hello World</title>\r\n</head>\r\n<body>\r\n	<h1>Error</h1>\r\n</body>\r\n</html>\r\n";
    // }
}

std::string responce::getHttp_responce( void )
{
    return (http_responce);
}

responce::responce()
{

}

responce::~responce()
{

}

responce::responce(const responce& copy)
{
    *this = copy;
}

responce& responce::operator=(const responce& obj)
{
    if (this != &obj)
    {
        http_request = obj.http_request;
        worker = obj.worker;
        http_responce = obj.http_responce;
    }
    return (*this);
}

std::string responce::Status(unsigned int status, std::map<unsigned int, std::string> &status_codes)
{
    std::string result;
    std::map<unsigned int, std::string>::iterator iter = status_codes.find(status);
    if (iter != status_codes.end())
    {
        std::stringstream   ss;
        ss << status;
        ss >> result;
        result += " " +iter->second;
    } 
    return (result);
}
