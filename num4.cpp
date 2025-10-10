#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];

    std::ifstream in_file(input_file);
    if (!in_file.is_open())
    {
        std::cerr << "Ошибка: не удалось открыть файл " << input_file << std::endl;
        return 1;
    }

    std::string line;
    std::getline(in_file, line);
    in_file.close();

    std::vector<int> numbers;
    std::stringstream ss(line);
    int num;
    while (ss >> num)
    {
        numbers.push_back(num);
    }

    int count_zero = 0;
    int count_ones = 0;
    if (!numbers.empty())
    {
        int current = numbers[0];
        int i = 0;
        int n = numbers.size();

        while (i < n)
        {
            current = numbers[i];

            while (i < n && numbers[i] == current)
            {
                i++;
            }

            if (current == 0)
            {
                count_zero++;
            } else
            {
                count_ones++;
            }
        }
    }

    std::ofstream out_file(output_file);
    if (!out_file.is_open())
    {
        std::cerr << "Ошибка: не удалось открыть файл " << output_file << std::endl;
        return 1;
    }

    out_file << count_zero << " " << count_ones;
    out_file.close();

    return 0;
}