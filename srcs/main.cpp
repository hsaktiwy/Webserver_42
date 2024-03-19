/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 12:03:40 by adardour          #+#    #+#             */
/*   Updated: 2024/03/19 22:09:03 by aalami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/http.server.hpp"

bool check_quote(const std::string &line, char &qoute)
{
    int i = 0;
    while (line[i])
    {
        if (line[i] == '\'' || line[i] == '\"')
        {
            qoute = line[i];
            return true;
        }
        i++;
    }
    return false;
}

void    readAndParseConfig(int c, char **argv,tokens_iterator &lines)
{
    int line_number = 1;

    std::string     path;
    std::string     line;
    std::ifstream   configfile;
    

    if (c == 1)
        path = DEFAULT_CONFIG_PATH;
    else 
        path = argv[1];

    std::ifstream file(path.c_str());
    if (file.is_open())
    { 
        std::string line;
        while (std::getline(file, line))
        { 
            lines.push_back(std::make_pair(line + '\n',line_number));
            line_number++;
        }
        file.close(); 
    }
    else
    {
        std::cout << "Unable to open file" << std::endl;
        exit(1);
    }
}

void show_info(std::vector<ServerBlocks> &blocks) {
    
    printf("\nSERVER INFO\t\n");
    for (size_t i = 0; i < blocks.size(); i++) {
        size_t j = 0;
        std::cout << "\x1B[1;34m"; 
        printf("server #%lu ", i);
        std::cout << "\x1B[0m";

        while (j < blocks[i].getDirectives().size()) {
            if (!blocks[i].getDirectives()[j].getDirective().compare("server_names"))
            {
                std::cout << "\x1B[1;32m"; 
                printf("Server Name %s", blocks[i].getDirectives()[j].getArgument()[0].c_str());
                std::cout << "\x1B[0m"; 
            }
            if (!blocks[i].getDirectives()[j].getDirective().compare("listen"))
            {
                std::cout << "\t\x1B[1;32m";
                printf("Host:Port %s\n", blocks[i].getDirectives()[j].getArgument()[0].c_str());
                std::cout << "\x1B[0m"; 
            }
            j++;
        }
        printf("\n");
    }
    printf("\n");
}

void    iniStatus_codes(std::map<unsigned int, std::string> &status_codes)
{
    status_codes.insert(std::make_pair(100, "Continue"));
    status_codes.insert(std::make_pair(101, "Switching Protocols"));
    status_codes.insert(std::make_pair(102, "Processing"));
    status_codes.insert(std::make_pair(103, "Early Hints"));
    status_codes.insert(std::make_pair(200, "OK"));
    status_codes.insert(std::make_pair(201, "Created"));
    status_codes.insert(std::make_pair(202, "Accepted"));
    status_codes.insert(std::make_pair(203, "Non-Authoritative Information"));
    status_codes.insert(std::make_pair(204, "No Content"));
    status_codes.insert(std::make_pair(205, "Reset Content"));
    status_codes.insert(std::make_pair(206, "Partial Content"));
    status_codes.insert(std::make_pair(207, "Multi-Status"));
    status_codes.insert(std::make_pair(208, "Already Reported"));
    status_codes.insert(std::make_pair(226, "IM Used"));
    status_codes.insert(std::make_pair(300, "Multiple Choices"));
    status_codes.insert(std::make_pair(301, "Moved Permanently"));
    status_codes.insert(std::make_pair(302, "Found"));
    status_codes.insert(std::make_pair(303, "See Other"));
    status_codes.insert(std::make_pair(304, "Not Modified"));
    status_codes.insert(std::make_pair(307, "Temporary Redirect"));
    status_codes.insert(std::make_pair(308, "Permanent Redirect"));
    status_codes.insert(std::make_pair(400, "Bad Request"));
    status_codes.insert(std::make_pair(401, "Unauthorized"));
    status_codes.insert(std::make_pair(402, "Payment Required"));
    status_codes.insert(std::make_pair(403, "Forbidden"));
    status_codes.insert(std::make_pair(404, "Not Found"));
    status_codes.insert(std::make_pair(405, "Method Not Allowed"));
    status_codes.insert(std::make_pair(406, "Not Acceptable"));
    status_codes.insert(std::make_pair(407, "Proxy Authentication Required"));
    status_codes.insert(std::make_pair(408, "Request Timeout"));
    status_codes.insert(std::make_pair(409, "Conflict"));
    status_codes.insert(std::make_pair(410, "Gone"));
    status_codes.insert(std::make_pair(411, "Length Required"));
    status_codes.insert(std::make_pair(412, "Precondition Failed"));
    status_codes.insert(std::make_pair(413, "Content Too Large"));
    status_codes.insert(std::make_pair(414, "URI Too Long"));
    status_codes.insert(std::make_pair(415, "Unsupported Media Type"));
    status_codes.insert(std::make_pair(416, "Range Not Satisfiable"));
    status_codes.insert(std::make_pair(417, "Expectation Failed"));
    status_codes.insert(std::make_pair(418, "I'm a teapot"));
    status_codes.insert(std::make_pair(421, "Misdirected Request"));
    status_codes.insert(std::make_pair(422, "Unprocessable Content"));
    status_codes.insert(std::make_pair(423, "Locked"));
    status_codes.insert(std::make_pair(424, "Failed Dependency"));
    status_codes.insert(std::make_pair(425, "Too Early"));
    status_codes.insert(std::make_pair(426, "Upgrade Required"));
    status_codes.insert(std::make_pair(428, "Precondition Required"));
    status_codes.insert(std::make_pair(429, "Too Many Requests"));
    status_codes.insert(std::make_pair(431, "Request Header Fields Too Large"));
    status_codes.insert(std::make_pair(451, "Unavailable For Legal Reasons"));
    status_codes.insert(std::make_pair(500, "Internal Server Error"));
    status_codes.insert(std::make_pair(501, "Not Implemented"));
    status_codes.insert(std::make_pair(502, "Bad Gateway"));
    status_codes.insert(std::make_pair(503, "Service Unavailable"));
    status_codes.insert(std::make_pair(504, "Gateway Timeout"));
    status_codes.insert(std::make_pair(505, "HTTP Version Not Supported"));
    status_codes.insert(std::make_pair(506, "Variant Also Negotiates"));
    status_codes.insert(std::make_pair(507, "Insufficient Storage"));
    status_codes.insert(std::make_pair(508, "Loop Detected"));
    status_codes.insert(std::make_pair(510, "Not Extended"));
    status_codes.insert(std::make_pair(511, "Network Authentication Required"));
}

int main(int c,char **argv)
{   
    if (c > 2)
    {
        printf("not a valid argument\n");
        exit(1);
    }
    tokens_iterator lines;
    tokens_map tokens;
    std::vector<ServerBlocks> serverBlocks;
    std::map<unsigned int, std::string> status_codes;
 
    signal(SIGPIPE, SIG_IGN);
    try
    {
        iniStatus_codes(status_codes);
        readAndParseConfig(c,argv,lines);
        parse_config(lines,tokens);
        handle_errors(tokens);
        proccess_tokens(tokens,serverBlocks);
        check_duplications(serverBlocks);
        start_listening_and_accept_request(serverBlocks, status_codes);
    }
    catch(std::string & e)
    {
        std::cerr << e << '\n';
    }
    catch( const char* e)
    {
        std::cerr << e << '\n';
    }
    return (0);
}