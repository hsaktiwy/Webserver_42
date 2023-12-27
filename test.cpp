#include <iostream>
#include <cmath>

int main(int argc, char **argv)
{
    if (argc == 9)
    {
        int s;
        int x = std::atoi(argv[2]),y = std::atoi(argv[4]),x2 = std::atoi(argv[6]),y2 = std::atoi(argv[8]);
        s = (x2 - x) *(x2 - x)+(y2 - y) * (y2 - y);
        s = std::sqrt(s);
        std::cout << s << std::endl;
    }
    else
        std::cout << "invalide argc\n";
    return 0;
}