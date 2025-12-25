#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>

namespace fs = std::filesystem;

// Структура для хранения параметров командной строки
struct Config 
{
    fs::path directory;
    std::string search_pattern;
    std::string replace_pattern;
    bool use_regex = false;
    bool recursive = true;
};

// Функция для разбора аргументов командной строки
Config parseArguments(int argc, char* argv[]) 
{
    Config config;
    
    if (argc < 4) 
    {
        throw std::invalid_argument("Недостаточно аргументов. Использование: rrename <директория> [флаги] <поиск> <замена>");
    }
    
    int arg_index = 1;
    
    // Первый аргумент - директория
    config.directory = argv[arg_index++];
    
    // Обработка флагов
    while (arg_index < argc - 2) 
    {
        std::string flag = argv[arg_index];
        if (flag == "-r") {
            config.use_regex = true;
        } else if (flag == "-nr") {
            config.recursive = false;
        } else {
            throw std::invalid_argument("Неизвестный флаг: " + flag);
        }
        arg_index++;
    }
    
    // Последние два аргумента - поиск и замена
    if (arg_index >= argc - 1) 
    {
        throw std::invalid_argument("Недостаточно аргументов для поиска и замены");
    }
    
    config.search_pattern = argv[arg_index];
    config.replace_pattern = argv[arg_index + 1];
    
    return config;
}

// Функция для преобразования простого шаблона в регулярное выражение
std::string wildcardToRegex(const std::string& pattern) 
{
    std::string regex_pattern;
    
    for (char c : pattern) 
    {
        switch (c) 
        {
            case '*':
                regex_pattern += ".*";
                break;
            case '?':
                regex_pattern += ".";
                break;
            case '.':
            case '^':
            case '$':
            case '+':
            case '|':
            case '{':
            case '}':
            case '[':
            case ']':
            case '\\':
            case '(':
            case ')':
                regex_pattern += '\\';
                regex_pattern += c;
                break;
            default:
                regex_pattern += c;
                break;
        }
    }
    
    return regex_pattern;
}

// Функция для выполнения замены с использованием регулярных выражений
std::string regexReplace(const std::string& input, const std::string& search_pattern, const std::string& replace_pattern, bool use_regex) 
{
    std::string actual_search = use_regex ? search_pattern : wildcardToRegex(search_pattern);
    
    try 
    {
        std::regex pattern(actual_search);
        return std::regex_replace(input, pattern, replace_pattern);
    } catch (const std::regex_error& e) 
    {
        throw std::runtime_error("Ошибка в регулярном выражении: " + std::string(e.what()));
    }
}

// Функция для переименования файлов
void renameFiles(const Config& config) 
{
    if (!fs::exists(config.directory)) 
    {
        throw std::runtime_error("Директория не существует: " + config.directory.string());
    }
    
    if (!fs::is_directory(config.directory)) 
    {
        throw std::runtime_error("Указанный путь не является директорией: " + config.directory.string());
    }
    
    std::vector<std::pair<fs::path, fs::path>> rename_operations;

    // Сначала собираем все операции переименования
    if (config.recursive) 
    {
        for (const auto& entry : fs::recursive_directory_iterator(config.directory)) 
        {
            if (entry.is_regular_file() || entry.is_directory()) 
            {
                std::string old_name = entry.path().filename().string();
                std::string new_name = regexReplace(old_name, config.search_pattern, config.replace_pattern, config.use_regex);
                
                if (old_name != new_name) 
                {
                    fs::path new_path = entry.path().parent_path() / new_name;
                    rename_operations.emplace_back(entry.path(), new_path);
                }
            }
        }
    } else 
    {
        for (const auto& entry : fs::directory_iterator(config.directory)) 
        {
            if (entry.is_regular_file() || entry.is_directory()) 
            {
                std::string old_name = entry.path().filename().string();
                std::string new_name = regexReplace(old_name, config.search_pattern, config.replace_pattern, config.use_regex);
                
                if (old_name != new_name) 
                {
                    fs::path new_path = entry.path().parent_path() / new_name;
                    rename_operations.emplace_back(entry.path(), new_path);
                }
            }
        }
    }
    
    // Выполняем переименования
    for (const auto& operation : rename_operations) 
    {
        try 
        {
            fs::rename(operation.first, operation.second);
            std::cout << "Переименован: " << operation.first.filename().string() 
                      << " -> " << operation.second.filename().string() << std::endl;
        } catch (const fs::filesystem_error& e) 
        {
            std::cerr << "Ошибка при переименовании " << operation.first << ": " << e.what() << std::endl;
        }
    }
    
    std::cout << "Переименовано файлов: " << rename_operations.size() << std::endl;
}

int main(int argc, char* argv[]) {
    try 
    {
        Config config = parseArguments(argc, argv);
        renameFiles(config);
        
    } catch (const std::exception& e) 
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}