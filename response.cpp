#include "response.hpp"

response::response(request &req, Worker &wk): http_request(&req), worker(&wk), body_index(0), body_size(-1), header_index(0), header_size(-1), body_sent(0), header_sent(0), FileOpened(false), fd(-1)
{

}

std::string TimeToString(timespec time)
{
    struct tm *data = std::gmtime(&time.tv_sec);
    std::string Year,Month,Day;
    int year, mon,day;
    year = 1900 + data->tm_year;
    mon = data->tm_mon + 1;
    day = data->tm_mday;
    std::stringstream ss;
    ss << year;
    ss >> Year;
    ss.str("");
    ss.clear();
    ss << mon;
    ss >> Month;
    ss.str("");
    ss.clear();
    ss << day;
    ss >> Day;
    return (Year + "-" + Month + "-" + Day);
}

void    autoIndexing(request &req, Worker &wk,std::string &response_head, std::string &body, std::map<unsigned int, std::string> &status_codes)
{
    Uri const &uri = req.getUri();
    std::string path = uri.authority + "/" +uri.path;
    std::string File_name, Last_modification, Size, Link, Path;
    Path = wk.getRoot() + req.getUri().path;
    DIR     *dir = opendir(Path.c_str());
    struct  dirent *dirent;

    response_head = "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=UTF-8\r\nServer: " + ((std::string)SERVERNAME) + "\r\n";
    body = "<table>\n<thead>\n    <tr>\n        <th>File Name</th>\n        <th>Last modification</th>\n        <th>Size</th>\n    </tr>\n</thead>\n<tbody>";
    if (dir)
    {
        while ((dirent = readdir(dir)) != NULL)
        {
            if (std::strcmp(dirent->d_name, ".") != 0)
            {
                Link = "/" + req.getUri().path + "/" + dirent->d_name;
                
                File_name = dirent->d_name,Size = "-", Last_modification = "-";
                Link = NormilisePath(Link);
                std::string tmpPath =  Path + "/" + dirent->d_name;
                struct stat File_state;
                if (stat(tmpPath.c_str(), &File_state) == 0)
                {
                    std::stringstream ss;
                    ss << File_state.st_size;
                    ss >> Size;
                    Last_modification = TimeToString(File_state.st_mtimespec);
                }
                std::string line = "\n<tr>\n<td><a href=\"" +  Link +"\">"+ File_name +"</a></td>\n<td>" + Last_modification + " </td>\n<td>"+ Size +"</td>\n</tr>";
                body += line;
            }
        }
        closedir(dir);
    }
    body += "\n</tbody>\n</table>";
    // printf("======================>AutoIndexing\n");
    std::stringstream ss;
    std::string body_size;
    ss << body.size();
    ss >> body_size;
    response_head += "Content-Length: " + body_size + "\r\n\r\n";
}


std::string GetFileType(const std::string &Path)
{
    size_t pos =  Path.rfind('.');
    if (pos != std::string::npos)
    {
        std::string extension = &Path[pos + 1];
        return (mime_types(extension));
    }
    return ("text/plain");
}

void    response::responed(std::map<unsigned int, std::string> &status_codes)
{
    std::string index = (*worker).getIndex();
    request &req = *http_request;
    Worker &wk = *worker;

    // Redirection Case
    if (!wk.getRedirect().empty())
    {
        std::string path =  wk.getRedirect();
        RedirectionResponse(status_codes, path);
        return ;
    }

    if (req.getError() == false)
    {
        // printf("--------->%d %d\n", req.getIs_dir(), req.getIs_regular());
        if (req.getIs_dir() == 1 || req.getIs_regular() == 1)
        {
            if (req.getIs_dir() == 1 &&  wk.getAutoIndex() == "on")
            {
                // autoindexing
                autoIndexing(req, wk,http_response, body_string, status_codes);
                header_size = http_response.size();
                body_size = body_string.size();
                return ;
            }
            else if (req.getIs_dir() == 1 && index.size() == 0)
            {
                req.setError(true), req.setStatus(404);
            }
        }
        else
        {
            req.setError(true), req.setStatus(404);
        }
    }
    // responed using error pages if  (error  == true)
    if (req.getError() == true)
    {
        // response for Error handling
        errorresponse(status_codes);
    }
    else
    {
        if (req.getMethod() == "GET")
        {
            FileType = GetFileType(req.getUri().path);
            int Range_index = http_request->getHeaderIndex("Range");
            std::string Code;
            if (Range_index == -1)
                Code =  Status(200, status_codes);
            else
                Code =  Status(206, status_codes);
            http_response = "HTTP/1.1 "+ Code +"\r\nContent-Type: " + FileType + "\r\n";
            file = wk.getRoot() + req.getUri().path;
            header_size = http_response.size();
        }
        else
        {
            body_string = "<html>\r\n<head>\r\n	<title>Valide File</title>\r\n</head>\r\n<body>\r\n	<h1>The response must be here!.</h1>\r\n</body>\r\n</html>\r\n";
            body_size = body_string.size();
            http_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nServer: " + ((std::string)SERVERNAME) + "Content-Length: " + ToString(body_size) + "\r\n\r\n";
            header_size = http_response.size();

        }
    }
}

void    response::RedirectionResponse(std::map<unsigned int, std::string> &status_codes, std::string &path)
{
    Worker &wk = *worker;
    request &req = *http_request;
    std::string HumanRead;
    std::stringstream ss;
    std::string statusCode;

    req.setStatus(301);
    std::map<unsigned int, std::string>::iterator iter = status_codes.find(req.getStatus());
    ss << req.getStatus();
    ss >> statusCode;
    if (iter != status_codes.end())
        HumanRead = iter->second;
    http_response = "HTTP/1.1 " + statusCode + " " + HumanRead + "\r\n" + "Content-Type: text/html\r\n" + "Location: "+ path + "\r\nServer: " + ((std::string)SERVERNAME) + "\r\n";
    header_size = http_response.size();
    body_size = 0;
    printf("Responcse in redirect mode \n%s\n", http_response.c_str());
}

void    response::errorresponse(std::map<unsigned int, std::string> &status_codes)
{
    Worker &wk = *worker;
    request &req = *http_request;
    std::string HumanRead;
    std::stringstream ss;
    std::string statusCode;

    worker->setPathError(worker->getErrorPages(), req.getStatus(),worker->getRoot());
    if (wk.get_track_status() == 0 || (wk.get_track_status() == 1 && wk.getPathError().empty()))
    {
        // printf("::::::::::::%d __ %s \n", wk.get_track_status(), wk.getPathError().c_str());
        if (wk.get_track_status() == 1 && wk.getPathError() == "")
            req.setStatus(404);
        std::map<unsigned int, std::string>::iterator iter = status_codes.find(req.getStatus());
        ss << req.getStatus();
        ss >> statusCode;
        if (iter != status_codes.end())
            HumanRead = iter->second;
        // this maybe need to be in a folder i mean like a error_page file but with out being define in the http configue file
        body_string = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Error Page</title>\r\n<style>\r\nbody {\r\nfont-family: Arial, sans-serif;\r\ntext-align: center;\r\npadding-top: 50px;\r\n}\r\nh1 {\r\nfont-size: 3em;\r\ncolor: #990000;\r\nmargin-bottom: 20px;\r\n}\r\np {\r\nfont-size: 1.5em;\r\ncolor: #666666;\r\nmargin-bottom: 50px;\r\n}\r\n</style>\r\n</head>\r\n<body>\r\n<h1>Error "+ statusCode + "("+ HumanRead +")"+"</h1>\r\n<p>Unhable to reserve a propore response.</p>\r\n</body>\r\n</html>";
        http_response += "HTTP/1.1 " + statusCode + " " + HumanRead + "\r\n";
        http_response += "Content-Type: text/html\r\n";
        std::string size;
        ss << body_string.size();
        ss >> size;
        http_response += "Content-Length: " + size + "\r\nServer: " + ((std::string)SERVERNAME) + "\r\n\r\n";
        body_size = body_string.size();
        header_size = http_response.size();
    }
    else
    {
        std::string path = wk.getLocationWorker().getPath() + "/" + worker->getPathError();
        path = NormilisePath(path);
        RedirectionResponse(status_codes, path);
    }
    // {
    //     // req.setStatus(301);
    //     // std::map<unsigned int, std::string>::iterator iter = status_codes.find(req.getStatus());
    //     // ss << req.getStatus();
    //     // ss >> statusCode;
    //     // if (iter != status_codes.end())
    //     //     HumanRead = iter->second;
    //     // std::string path = wk.getLocationWorker().getPath() + "/" + worker->getPathError();
    //     // path = NormilisePath(path);
    //     // http_response = "HTTP/1.1 " + statusCode + " " + HumanRead + "\r\n" + "Content-Type: text/html\r\n" + "Location: http://" + req.getHost() + "/" + path + "\r\n";
    //     // header_size = http_response.size();
    //     // body_size = 0;
    // }
}

std::string response::getHttp_response( void )
{
    return (http_response);
}

response::response():http_request(NULL), worker(NULL), body_index(0), body_size(-1), header_index(0), header_size(-1), body_sent(0), header_sent(0), FileOpened(false), fd(-1)
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
        printf("Copy Assigned oPERATOR IS CALLED\n");
        printf("Before setting ->%lld %lu\n", header_size, header_index);
        printf("To Copy setting %lld %lu\n", obj.header_size, obj.header_index);

        http_response = obj.http_response;
        http_request = obj.http_request;
        body_string = obj.body_string;
        file = obj.file;
        header_index = obj.header_index;
        body_index = obj.body_index;
        FileIndex = obj.FileIndex;
        FileEnd = obj.FileEnd;
        FileType = obj.FileType;
        header_size = obj.header_size;
        body_size = obj.body_size;
        header_sent = obj.header_sent;
        body_sent = obj.body_sent;
        FileOpened = obj.FileOpened;
        fd = obj.fd;
        printf("After setting %lld %lu\n", header_size, header_index);
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


void        response::setHttp_request(request &request)
{
    http_request = &request;
}

void        response::setWorker(Worker &wk)
{
    worker = &wk;
}

size_t      response::getHeader_index( void ) const
{
        return (header_index);
}

long long   response::getHeader_size( void ) const
{
    return (header_size);
}

long long   response::getBody_size( void ) const
{
    return (body_size);
}

size_t      response::getBody_index( void ) const
{
    return (body_index);
}

bool        response::getHeader_sent( void ) const
{
    return (header_sent);
}

bool        response::getBody_sent( void ) const
{
    return (body_sent);
}

bool    response::getFileOpened( void ) const
{
    return (FileOpened);
}

int     response::getFd( void ) const
{
    return (fd);
}

std::string response::getFileType( void ) const
{
    return (FileType);
}

void        response::setHeader_index(size_t value)
{
    header_index = value;
}

void        response::setHeader_size(long long  value)
{
    header_size = value;
}

void        response::setBody_size(long long  value)
{
    body_size = value;
}

void        response::setBody_index(size_t  value)
{
    body_index = value;
}

void        response::setHeader_sent(bool  value)
{
    header_sent = value;
}

void        response::setBody_sent( bool  value)
{
    body_sent = value;
}


void    response::setFileOpened( bool value )
{
    FileOpened = value;
}

void     response::setFd( int value )
{
    fd = value;
}

size_t      response::getFileIndex( void ) const
{
    return (FileIndex);
}

size_t      response::getFileEnd( void ) const
{
    return (FileEnd);
}

void        response::setFileIndex(size_t value)
{
    FileIndex = value;
}

void        response::setFileEnd(size_t value)
{
    FileEnd = value;
}
