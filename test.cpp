#include <iostream>

int main()
{
    std::string tmp;
    std::string data;

    while (getline(std::cin, tmp, '\n'))
    {
        if (tmp == "exit")
        {
            std::cout << data;
            return (0);
        }
        data += tmp + "\r\n";
    }
    return (0);
}