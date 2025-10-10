#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <algorithm>
#include <functional>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1]);
    if (!input_file.is_open())
    {
        std::cerr << "Ошибка открытия файла: " << argv[1] << std::endl;
        return 1;
    }

    int N;
    input_file >> N;

    std::vector<int> cards(N);
    for (int i = 0; i < N; ++i)
    {
        input_file >> cards[i];
    }
    input_file.close();

    std::partial_sort(cards.begin(), cards.begin() + N/2, cards.end(), std::greater<int>());

    int max_sum = std::accumulate(cards.begin(), cards.begin() + N/2, 0);
    std::sort(cards.begin() + N/2, cards.end());
    int min_sum = std::accumulate(cards.begin() + N/2, cards.end(), 0);
    int max_win = max_sum - min_sum;

    std::ofstream output_file(argv[2]);
    if (!output_file.is_open())
    {
        std::cerr << "Ошибка открытия файла " << argv[2] << std::endl;
        return 1;
    }
    output_file << max_win << std::endl;
    output_file.close();

    return 0;
}