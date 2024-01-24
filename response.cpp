#include "response.hpp"

response::response(request &req, Worker &wk): http_request(&req), worker(&wk)
{
    
}

void    autoIndexing(request &req, std::string &response_head, std::string &body, std::map<unsigned int, std::string> &status_codes)
{
    Uri const &uri = req.getUri();
    std::string path = uri.authority + "/" +uri.path;
    std::cout << path << std::endl;
    printf("Autoindexing\n");
    // response_head = "HTTP/1.1 200 OK\r\n" + "Content-Type: text/html;charset=UTF-8";
}

void    response::responed(std::map<unsigned int, std::string> &status_codes)
{
    std::string index = (*worker).getIndex();
    request &req = *http_request;
    Worker &wk = *worker;


    if (req.getError() == false)
    {
        printf("--------->%d %d\n", req.getIs_dir(), req.getIs_regular());
        if (req.getIs_dir() == 1 || req.getIs_regular() == 1)
        {
            if (req.getIs_dir() == 1 && index.size() == 0 &&  wk.getAutoIndex() == "on")
            {
                // autoindexing
                autoIndexing(req, http_response, body_string, status_codes);
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
        // response for Error handling
        errorresponse(status_codes);
    }
    else
        http_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html>\r\n<head>\r\n	<title>Valide File</title>\r\n</head>\r\n<body>\r\n	<h1>The response must be here!.</h1>\r\n</body>\r\n</html>\r\n";
}

void    response::errorresponse(std::map<unsigned int, std::string> &status_codes)
{
    Worker &wk = *worker;
    request &req = *http_request;
    std::string HumanRead;
    std::stringstream ss,ss2;
    std::string statusCode;

    worker->setPathError(worker->getErrorPages(), req.getStatus(),worker->getRoot());
    if (wk.get_track_status() == 0 || (wk.get_track_status() == 1 && wk.getPathError().empty()))
    {
        printf("::::::::::::%d __ %s \n", wk.get_track_status(), wk.getPathError().c_str());
        if (wk.get_track_status() == 1 && wk.getPathError() == "")
            req.setStatus(404);
        std::map<unsigned int, std::string>::iterator iter = status_codes.find(req.getStatus());
        ss << req.getStatus();
        ss >> statusCode;
        if (iter != status_codes.end())
            HumanRead = iter->second;
        body_string = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Error Page</title>\r\n<style>\r\nbody {\r\nfont-family: Arial, sans-serif;\r\ntext-align: center;\r\npadding-top: 50px;\r\n}\r\nh1 {\r\nfont-size: 3em;\r\ncolor: #990000;\r\nmargin-bottom: 20px;\r\n}\r\np {\r\nfont-size: 1.5em;\r\ncolor: #666666;\r\nmargin-bottom: 50px;\r\n}\r\n</style>\r\n</head>\r\n<body>\r\n<h1>Error "+ statusCode + "("+ HumanRead +")"+"</h1>\r\n<p>Unhable to reserve a propore response.</p>\r\n</body>\r\n</html>";
        http_response += "HTTP/1.1 " + statusCode + " " + HumanRead + "\r\n";
        http_response += "Content-Type: text/html\r\n";
        std::string size;
        ss2 << body_string.size();
        ss2 >> size;
        http_response += "Content-Length: " + size + "\r\n\r\n";
    }
    else
    {
        printf("solo\n");
        std::map<unsigned int, std::string>::iterator iter = status_codes.find(req.getStatus());
        ss << 307;
        ss >> statusCode;
        if (iter != status_codes.end())
            HumanRead = iter->second;
        // this will be handled when the error_page directive will be ready
        http_response = "HTTP/1.1 " + statusCode + " " + HumanRead + "\r\n" + "Content-Type: text/html\r\n" + "Location: http://" + req.getHost() + "/" + worker->getRoot() + "/" + worker->getPathError() + "\r\n";
    }
}

std::string response::getHttp_response( void )
{
    return (http_response);
}

response::response()
{

}

response::~response()
{

}

response::response(const response& copy)
{
    *this = copy;
}

response& response::operator=(const response& obj)
{
    if (this != &obj)
    {
        http_request = obj.http_request;
        worker = obj.worker;
        http_response = obj.http_response;
    }
    return (*this);
}

std::string response::Status(unsigned int status, std::map<unsigned int, std::string> &status_codes)
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

std::string response::getHttp_response( void ) const
{
    return (http_response);
}

std::string response::getBody_string( void ) const
{
    return (body_string);
}

std::string response::getFile( void ) const
{
    return (file);
}
