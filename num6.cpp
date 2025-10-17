#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1]);
    if (!input_file.is_open())
    {
        std::cerr << "Ошибка открытия входного файла " << argv[1] << std::endl;
        return 1;
    }

    int N, a, b;
    input_file >> N >> a >> b;

    std::vector<int> original(N);
    for (int i = 0; i < N; ++i)
    {
        input_file >> original[i];
    }
    input_file.close();

    std::ofstream output_file(argv[2]);
    if (!output_file.is_open())
    {
        std::cerr << "Ошибка открытия файла " << argv[2] << std::endl;
        return 1;
    }

    output_file << "Исходный массив:";
    std::copy(original.begin(), original.end(), std::ostream_iterator<int>(output_file, " "));
    output_file << std::endl;

    std::vector<int> in_range = original;
    auto new_end = std::remove_if(in_range.begin(), in_range.end(), [a, b](int x) { return x < a || x > b; });
    in_range.erase(new_end, in_range.end());

    output_file << "Массив в диапазоне [" << a << ", " << b << "]: ";
    std::copy(in_range.begin(), in_range.end(), std::ostream_iterator<int>(output_file, " "));
    output_file << std::endl;

    if (in_range.empty())
    {
        output_file << "Массив пуст после фильтрации" << std::endl;
        output_file.close();
        return 0;
    }

    auto max_it = max_element(in_range.begin(), in_range.end());

    auto first_after_max = std::find_if(std::next(max_it), in_range.end(), [](int x) { return x % 2 == 0;});

    std::vector<int> final_arr = in_range;
    int removed_value = 0;
    int removed_pos = 0;

    if (first_after_max != in_range.end())
    {
        removed_value = *first_after_max;
        removed_pos = std::distance(in_range.begin(), first_after_max);
        auto it_to_remove = first_after_max;
        auto remove_pred = [it_to_remove](int& x) { return &x == &(*it_to_remove);};
        new_end = std::remove_if(final_arr.begin(), final_arr.end(), remove_pred);
        final_arr.erase(new_end, final_arr.end());
    }

    output_file << "Массив после исключения элемента: ";
    std::copy(final_arr.begin(), final_arr.end(), std::ostream_iterator<int>(output_file, " "));
    output_file << std::endl;

    if (removed_pos != -1)
    {
        output_file << "Позиция исключенного элемента: " << removed_pos 
                    << ", значение: " << removed_value << std::endl;
    } else
    {
        output_file << "Элемент для исключения не найден" << std::endl;
    }

    int positive_count = std::count_if(final_arr.begin(), final_arr.end(), [](int x) { return x > 0; });
    int negative_count = std::count_if(final_arr.begin(), final_arr.end(), [](int x) { return x < 0; });
    output_file << "Количество положительных чисел: " << positive_count << std::endl;
    output_file << "Количество отрицательных чисел: " << negative_count << std::endl;

    output_file.close();
    return 0;
}
