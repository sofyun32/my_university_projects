#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <functional>
#include <cctype>
#include <set>

// Структура для хранения варианта анаграммы
struct AnagramVariant 
{
    std::string word;      // Слово
    double weight;         // Вес слова
    
    AnagramVariant(const std::string& w, double wt) : word(w), weight(wt) {}
};

// Структура для хранения результата расшифровки
struct DecryptionResult 
{
    double totalWeight;    // Суммарный вес
    std::string sentence;  // Расшифрованное предложение
    
    DecryptionResult(double w, const std::string& s) : totalWeight(w), sentence(s) {}
};

// Класс для работы со словарем анаграмм
class AnagramDictionary 
{
private:
    // Основной словарь: слово -> вес
    std::map<std::string, double> wordDictionary;
    
    // Индекс анаграмм: ключ (отсортированные буквы) -> список вариантов
    std::map<std::string, std::vector<AnagramVariant>> anagramIndex;
    
    // Вспомогательная функция: создает ключ для анаграммы
    std::string createAnagramKey(const std::string& word) 
    {
        std::string key = word;
        // Приводим к нижнему регистру
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        // Сортируем буквы
        std::sort(key.begin(), key.end());
        return key;
    }
    
    // извлекаем данные из строки словаря
    bool extractDictionaryEntry(const std::string& line, std::string& word, double& weight) 
    {
        // Удаляем все пробелы из строки
        std::string cleanLine;
        for (char c : line) 
        {
            if (c != ' ' && c != '\n' && c != '\r') 
            {
                cleanLine += c;
            }
        }
        
        // Ищем паттерн (слово,вес)
        size_t openParen = cleanLine.find('(');
        if (openParen == std::string::npos) return false;
        
        size_t commaPos = cleanLine.find(',', openParen);
        if (commaPos == std::string::npos) return false;
        
        size_t closeParen = cleanLine.find(')', commaPos);
        if (closeParen == std::string::npos) return false;
        
        // Извлекаем слово и вес
        word = cleanLine.substr(openParen + 1, commaPos - openParen - 1);
        std::string weightStr = cleanLine.substr(commaPos + 1, closeParen - commaPos - 1);
        
        try 
        {
            weight = std::stod(weightStr);
            return true;
        } catch (...) {
            return false;
        }
    }
    
public:
    AnagramDictionary() = default;
    
    // Загрузка словаря из файла
    bool loadFromFile(const std::string& filename) 
    {
        std::ifstream file(filename);
        if (!file.is_open()) 
        {
            std::cerr << "Ошибка открытия файла '" << filename << "'" << std::endl;
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) 
        {
            if (line.empty()) continue;
            
            std::string word;
            double weight;
            
            if (extractDictionaryEntry(line, word, weight)) 
            {
                wordDictionary[word] = weight;
            }
        }
        
        file.close();
        buildAnagramIndex();
        return true;
    }
    
    // Построение индекса анаграмм
    void buildAnagramIndex() 
    {
        anagramIndex.clear();
        
        for (const auto& entry : wordDictionary) 
        {
            std::string key = createAnagramKey(entry.first);
            anagramIndex[key].push_back(AnagramVariant(entry.first, entry.second));
        }
        
        // Сортируем варианты по убыванию веса
        for (auto& pair : anagramIndex) 
        {
            std::sort(pair.second.begin(), pair.second.end(),
                [](const AnagramVariant& a, const AnagramVariant& b) 
                {
                    return a.weight > b.weight;
                });
        }
    }
    
    // Поиск вариантов анаграмм для слова
    std::vector<AnagramVariant> findAnagrams(const std::string& word) 
    {
        std::string key = createAnagramKey(word);
        auto it = anagramIndex.find(key);
        if (it != anagramIndex.end()) 
        {
            return it->second;
        }
        return {};
    }
};

// Класс для генерации перестановок
class PermutationGenerator 
{
public:
    // Генерация всех перестановок вектора строк
    static std::vector<std::vector<std::string>> generate(const std::vector<std::string>& items) 
    {
        std::vector<std::vector<std::string>> allPermutations;
        std::vector<std::string> mutableItems = items;
        
        // Сортируем для начала генерации
        std::sort(mutableItems.begin(), mutableItems.end());
        
        // Генерируем все перестановки
        do 
        {
            allPermutations.push_back(mutableItems);
        } while (std::next_permutation(mutableItems.begin(), mutableItems.end()));
        
        return allPermutations;
    }
};

// Основной класс для расшифровки предложений
class SentenceDecryptor 
{
private:
    AnagramDictionary dictionary;
    
    // Разбиение предложения на слова
    std::vector<std::string> splitIntoWords(const std::string& sentence) 
    {
        std::vector<std::string> words;
        std::istringstream stream(sentence);
        std::string word;
        
        while (stream >> word) 
        {
            words.push_back(word);
        }
        
        return words;
    }
    
    // Проверка возможности расшифровки всех слов
    bool canDecryptAllWords(const std::vector<std::string>& words) 
    {
        for (const auto& word : words) 
        {
            if (dictionary.findAnagrams(word).empty()) 
            {
                return false;
            }
        }
        return true;
    }
    
    // Генерация всех комбинаций вариантов слов
    std::vector<std::pair<double, std::vector<std::string>>> 
    generateWordCombinations(const std::vector<std::string>& words) 
    {
        
        // Получаем все варианты для каждого слова
        std::vector<std::vector<AnagramVariant>> wordOptions;
        for (const auto& word : words) 
        {
            wordOptions.push_back(dictionary.findAnagrams(word));
        }
        
        // Инициализируем начальные комбинации
        std::vector<std::pair<double, std::vector<std::string>>> combinations;
        combinations.push_back({0.0, {}});
        
        // Последовательно добавляем варианты слов
        for (const auto& options : wordOptions) 
        {
            std::vector<std::pair<double, std::vector<std::string>>> newCombinations;
            
            for (const auto& comb : combinations) 
            {
                for (const auto& option : options) 
                {
                    std::vector<std::string> newWords = comb.second;
                    newWords.push_back(option.word);
                    double newWeight = comb.first + option.weight;
                    newCombinations.push_back({newWeight, newWords});
                }
            }
            
            combinations = newCombinations;
        }
        
        return combinations;
    }
    
public:
    SentenceDecryptor(const AnagramDictionary& dict) : dictionary(dict) {}
    
    // Основной метод расшифровки
    std::vector<DecryptionResult> decrypt(const std::string& sentence) 
    {
        // Разбиваем предложение на слова
        std::vector<std::string> words = splitIntoWords(sentence);
        
        // Проверяем возможность расшифровки
        if (!canDecryptAllWords(words)) 
        {
            for (const auto& word : words) 
            {
                if (dictionary.findAnagrams(word).empty()) 
                {
                    std::cerr << "Error: no anagram '" << word << "'" << std::endl;
                }
            }
            return {};
        }
        
        // Генерируем все комбинации слов
        auto combinations = generateWordCombinations(words);
        
        // Генерируем перестановки и собираем результаты
        std::set<std::string> uniqueSentences;
        std::vector<DecryptionResult> results;
        
        for (const auto& comb : combinations) 
        {
            // Генерируем все перестановки для текущей комбинации
            auto permutations = PermutationGenerator::generate(comb.second);
            
            for (const auto& perm : permutations) 
            {
                // Собираем предложение из перестановки
                std::string sentenceStr;
                for (size_t i = 0; i < perm.size(); i++) 
                {
                    if (i > 0) sentenceStr += " ";
                    sentenceStr += perm[i];
                }
                
                // Проверяем уникальность
                if (uniqueSentences.find(sentenceStr) == uniqueSentences.end()) 
                {
                    uniqueSentences.insert(sentenceStr);
                    results.push_back(DecryptionResult(comb.first, sentenceStr));
                }
            }
        }
        
        // Сортируем результаты
        std::sort(results.begin(), results.end(),
            [](const DecryptionResult& a, const DecryptionResult& b) 
            {
                if (a.totalWeight != b.totalWeight) 
                {
                    return a.totalWeight > b.totalWeight;  // По убыванию веса
                }
                return a.sentence < b.sentence;            // По алфавиту
            });
        
        return results;
    }
};

int main(int argc, char* argv[]) 
{
    if (argc != 3) 
    {
        std::cerr << "arg error" << std::endl;
        return 1;
    }
    
    std::string inputFilename = argv[1];
    std::string dictFilename = argv[2];
    
    AnagramDictionary dictionary;
    if (!dictionary.loadFromFile(dictFilename)) 
    {
        return 1;
    }
    
    // Создаем дешифратор
    SentenceDecryptor decryptor(dictionary);
    
    // Открываем файл с анаграммами
    std::ifstream inputFile(inputFilename);
    if (!inputFile.is_open()) 
    {
        std::cerr << "Ошибка окрытия файла '" << inputFilename << "'" << std::endl;
        return 1;
    }
    
    // Обрабатываем каждую строку файла
    std::string line;
    int lineNumber = 1;
    
    while (std::getline(inputFile, line)) 
    {
        // Пропускаем пустые строки
        if (line.empty()) continue;
        
        // Удаляем пробелы в начале и конце строки
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) continue;
        
        // Выводим информацию о текущей строке 
        std::cout << "\nLine " << lineNumber << ": \"" << line << "\"" << std::endl;
        std::cout << "==========================================" << std::endl;
        
        // Расшифровываем строку
        auto decryptions = decryptor.decrypt(line);
        
        // Выводим результаты 
        if (decryptions.empty())
        {
            std::cout << "No valid decryptions found" << std::endl;
        } else 
        {
            std::cout << "Found " << decryptions.size() << " decryption variants:" << std::endl;
            
            for (size_t i = 0; i < decryptions.size(); ++i) 
            {
                std::cout << i + 1 << ". " << decryptions[i].sentence 
                         << " | weight=" << decryptions[i].totalWeight << std::endl;
            }
        }
        
        lineNumber++;
    }
    
    inputFile.close();
    return 0;
}