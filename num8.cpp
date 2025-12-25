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

// читаем словарь
std::map<std::string, double> readDictionary(const std::string& filename) {
    std::map<std::string, double> dictionary;
    std::ifstream file(filename);
    std::string line;
    
    if (!file.is_open()) 
    {
        std::cerr << "Error open file'" << filename << "'" << std::endl;
        exit(1);
    }
    
    while (std::getline(file, line)) 
    {
        if (line.empty()) continue; // пропускаем пустые строки
        
        std::string processed_line;
        for (char c : line) 
        {
            if (c != ' ' && c != '\n' && c != '\r') 
            {
                processed_line += c; // обработанная строка
            }
        }
        
        size_t pos = 0;// начальная позиция
        while (pos < processed_line.length()) 
        {
            if (processed_line[pos] == '(') {
                size_t comma_pos = processed_line.find(',', pos);
                size_t end_pos = processed_line.find(')', pos);
                
                if (comma_pos != std::string::npos && end_pos != std::string::npos)
                {
                    std::string word = processed_line.substr(pos + 1, comma_pos - pos - 1);// извлекает строку
                    std::string weight_str = processed_line.substr(comma_pos + 1, end_pos - comma_pos - 1);// извлекаем вес
                    
                    try 
                    {
                        double weight = std::stod(weight_str);// делаем из строки число 
                        dictionary[word] = weight;
                    } 
                    
                    catch (const std::exception& e) 
                    {
                        // если преобразование не удалось, пропускаем запись
                    }
                    
                    pos = end_pos + 1;
                } 
                
                else 
                {
                    break;
                }
            } 

            else 
            {
                pos++;
            }
        }
    }
    
    file.close();
    return dictionary;
}

// создаём ключ для анаграмм
std::string getAnagramKey(const std::string& word) 
{
    std::string key = word;
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);// приводим к нижнему регистру
    std::sort(key.begin(), key.end());// буквы в отсортированном порядке
    return key;
}

// Build anagram dictionary
std::map<std::string, std::vector<std::pair<std::string, double>>> buildAnagramDict(
    const std::map<std::string, double>& dictionary) {
    
    std::map<std::string, std::vector<std::pair<std::string, double>>> anagramDict;
    
    for (const auto& entry : dictionary) {
        std::string key = getAnagramKey(entry.first);
        anagramDict[key].push_back(std::make_pair(entry.first, entry.second));
    }
    
    // Sort variants by weight (descending)
    for (auto& entry : anagramDict) {
        std::sort(entry.second.begin(), entry.second.end(), 
             [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
                 return a.second > b.second;
             });
    }
    
    return anagramDict;
}

// Find all possible decryption variants for a word
std::vector<std::pair<std::string, double>> findAnagramVariants(
    const std::string& word, 
    const std::map<std::string, std::vector<std::pair<std::string, double>>>& anagramDict) {
    
    std::string key = getAnagramKey(word);
    auto it = anagramDict.find(key);
    if (it != anagramDict.end()) {
        return it->second;
    }
    return {};
}

// Упрощенная функция генерации всех перестановок с использованием std::next_permutation
std::vector<std::vector<std::string>> generateAllPermutations(const std::vector<std::string>& items) {
    std::vector<std::vector<std::string>> permutations;
    std::vector<std::string> temp = items;
    
    // Сначала сортируем, чтобы next_permutation работал корректно
    std::sort(temp.begin(), temp.end());
    
    do {
        permutations.push_back(temp);
    } while (std::next_permutation(temp.begin(), temp.end()));
    
    return permutations;
}

// Main decryption function
std::vector<std::pair<double, std::string>> decryptSentence(
    const std::string& sentence,
    const std::map<std::string, std::vector<std::pair<std::string, double>>>& anagramDict) {
    
    std::vector<std::string> inputWords;
    std::istringstream iss(sentence);
    std::string word;
    
    while (iss >> word) {
        inputWords.push_back(word);
    }
    
    // Check if all words have at least one variant
    for (const auto& w : inputWords) {
        if (findAnagramVariants(w, anagramDict).empty()) {
            std::cerr << "Error: No dictionary entry for anagram '" << w << "'" << std::endl;
            return {};
        }
    }
    
    // Get ALL possible word choices for each position
    std::vector<std::vector<std::pair<std::string, double>>> allWordChoices;
    for (const auto& w : inputWords) {
        allWordChoices.push_back(findAnagramVariants(w, anagramDict));
    }
    
    // Generate all combinations of word choices
    std::vector<std::pair<double, std::vector<std::string>>> combinationsWithWeights;
    
    // Start with empty combination
    std::vector<std::vector<std::string>> allCombinations = {{}};
    std::vector<double> combinationWeights = {0.0};
    
    for (const auto& choices : allWordChoices) {
        std::vector<std::vector<std::string>> newCombinations;
        std::vector<double> newWeights;
        
        for (size_t i = 0; i < allCombinations.size(); i++) {
            for (const auto& choice : choices) {
                std::vector<std::string> newCombination = allCombinations[i];
                newCombination.push_back(choice.first);
                newCombinations.push_back(newCombination);
                newWeights.push_back(combinationWeights[i] + choice.second);
            }
        }
        
        allCombinations = newCombinations;
        combinationWeights = newWeights;
    }
    
    // For each combination, generate all permutations of word order using std::next_permutation
    std::set<std::string> uniqueResults;
    std::vector<std::pair<double, std::string>> results;
    
    for (size_t i = 0; i < allCombinations.size(); i++) {
        const auto& combination = allCombinations[i];
        double weight = combinationWeights[i];
        
        // Используем упрощенную функцию с std::next_permutation
        std::vector<std::vector<std::string>> permutations = generateAllPermutations(combination);
        
        for (const auto& perm : permutations) {
            std::string resultStr;
            for (size_t j = 0; j < perm.size(); j++) {
                if (j > 0) resultStr += " ";
                resultStr += perm[j];
            }
            
            if (uniqueResults.find(resultStr) == uniqueResults.end()) {
                uniqueResults.insert(resultStr);
                results.push_back(std::make_pair(weight, resultStr));
            }
        }
    }
    
    // Sort by weight descending
    std::sort(results.begin(), results.end(), 
              [](const std::pair<double, std::string>& a, const std::pair<double, std::string>& b) {
                  if (a.first != b.first) {
                      return a.first > b.first;
                  }
                  return a.second < b.second;
              });
    
    return results;
}

int main(int argc, char* argv[]) {
    if (argc != 3) 
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <dictionary_file>" << std::endl;
        return 1;
    }
    
    std::string inputFilename = argv[1];
    std::string dictFilename = argv[2];
    
    // Read dictionary
    std::map<std::string, double> dictionary = readDictionary(dictFilename);
    std::map<std::string, std::vector<std::pair<std::string, double>>> anagramDict = buildAnagramDict(dictionary);
    
    // Read input file with anagrams
    std::ifstream inputFile(inputFilename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file '" << inputFilename << "'" << std::endl;
        return 1;
    }
    
    std::string line;
    int lineNumber = 1;
    
    while (std::getline(inputFile, line)) {
        if (line.empty()) continue;
        
        // Trim the line
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) continue;
        
        std::cout << "\nLine " << lineNumber << ": \"" << line << "\"" << std::endl;
        std::cout << "==========================================" << std::endl;
        
        std::vector<std::pair<double, std::string>> decryptions = decryptSentence(line, anagramDict);
        
        if (decryptions.empty()) {
            std::cout << "No valid decryptions found" << std::endl;
        } else {
            std::cout << "Found " << decryptions.size() << " decryption variants:" << std::endl;
            for (size_t i = 0; i < decryptions.size(); ++i) {
                std::cout << i + 1 << ". " << decryptions[i].second << " | weight=" << decryptions[i].first << std::endl;
            }
        }
        
        lineNumber++;
    }
    
    inputFile.close();
    return 0;
}