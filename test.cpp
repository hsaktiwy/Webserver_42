#include <iostream>
#include <sstream>

int main()
{
    while (true)
    {
        std::string result;
        std::cin >> result;
        std::stringstream ss;
        ss << std::hex << (unsigned int)result[0];
        std::string r;
        ss >> r;
        std::cout << r;
    }
}