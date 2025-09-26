#include "num3.h"

int main()
{
    std::string input;
    std::cout << "Введите строку: ";
    std::getline(std::cin, input);

    int result = check(input);
    std::cout << (result == 0 ? "Success" : std::to_string(result)) << std::endl;

    return 0;
}