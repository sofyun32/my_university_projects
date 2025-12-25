#include <iostream>
#include <variant>
#include <string>
#include <vector>
#include <optional>
#include <cmath>
#include <cstring>

using Roots = std::variant<std::monostate, double, std::pair<double, double>>;

//Вычисление корней квадратного уравнения
Roots solve_quadratic(double a, double b, double c)
{
    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
    {
        return std::monostate{};
    } else if (std::abs(discriminant) < 1e-10)
    {
        double root = -b / (2 * a);
        return root;
    } else
    {
        double sqrt_d = std::sqrt(discriminant);
        double root1 = (-b - sqrt_d) / (2 * a);
        double root2 = (-b + sqrt_d) / (2 * a);
        return std::make_pair(root1, root2);
    }
}

struct RootVisitor
{
    void operator()(const std::monostate&) const
    {
        std::cout << "Нет вещественных корней" << std::endl;
    }

    void operator()(double root) const
    {
        std::cout << "Единственный корень: " << root << std::endl;
    }

    void operator()(const std::pair<double, double>& roots) const
    {
        std::cout << "Два корня: " << roots.first << " и " << roots.second << std::endl;
    }
};

std::optional<double> parse_command_args(int argc, char* argv[])
{
    double sum = 0.0;
    bool has_numbers = false;

    for (int i = 0; i < argc; ++i)
    {
        try
        {
            size_t pos;
            double value = std::stod(argv[i], &pos);

            //Провка, что вся строка преобразована в число
            if (pos == strlen(argv[i]))
            {
                sum += value;
                has_numbers = true;
            }
        }
        catch(const std::exception& e)
        {
        }  
    }

    return has_numbers ? std::optional<double>{sum} : std::nullopt;
}

int main(int argc, char* argv[])
{
    std::cout << "Решение вадратного уравнения:" << std::endl;
    auto roots1 = solve_quadratic(1, -3, 2);
    auto roots2 = solve_quadratic(1, -2, 1);
    auto roots3 = solve_quadratic(1, 2, 5);

    RootVisitor visitor;
    std::visit(visitor, roots1);
    std::visit(visitor, roots2);
    std::visit(visitor, roots3);

    std::cout <<"\nОбработка аргуметов командной строки:" << std::endl;
    if (auto res = parse_command_args(argc, argv))
    {
        std::cout << "Сумма номеров аргументов: " << *res << std::endl;
    } else
    {
        std::cout << "Не найдена сумма аргументов" << std::endl;
    }

    return 0;
}