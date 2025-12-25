#include <iostream>
#include <filesystem>
#include <string>
#include <regex>

struct Config 
{
    std::filesystem::path directory;   
    std::string search_pattern;           //шаблон для поиска в именах файлов
    std::string replace_pattern;          //шаблон для замены
    bool recursive = false;               //флаг рекурсивного поиска
};

//функция для разбора аргументов командной строки
Config parseArguments(int argc, char* argv[]) 
{
    Config config;
    
    if (argc < 4) 
    {
        throw std::invalid_argument("Недостаточное количество аргументов");
    }
    
    //первый аргумент - название папки
    config.directory = argv[1];
    
    //проверяем, является ли второе слово флагом
    int current_arg = 2;

    while (current_arg < argc && argv[current_arg][0] == '-') 
    {
        std::string flag = argv[current_arg];

        if (flag == "-r") 
        {
            config.recursive = true;      //включить рекурсивный обход
        } 
        
        else 
        {
            throw std::invalid_argument("Неизвестный флаг: " + flag); 
        }
        
        current_arg++;  //переход к следующему аргументу
    }
    
    if (argc - current_arg < 2) 
    {
        throw std::invalid_argument("Недостаточное количество аргументов для поиска и замены");
    }
    
    //сохранение шаблонов поиска и замены
    config.search_pattern = argv[current_arg]; //то, что ищем
    config.replace_pattern = argv[current_arg + 1]; //то, что меняем
    
    return config;
}

//функция для выполнения переименования
void performRename(const Config& config) 
{
    //создаём regex из шаблона поиска
    std::regex search_regex(config.search_pattern);
    
    auto processFile = [&](const std::filesystem::directory_entry& entry)
    {
        //пропускаем папки
        if (!entry.is_regular_file()) return;
        
        std::string old_name = entry.path().filename().string();
        
        std::string new_name = std::regex_replace(old_name, search_regex, config.replace_pattern);
        
        if (old_name != new_name) 
        {
            //создаем новый путь в той же папке
            std::filesystem::path new_path = entry.path().parent_path() / new_name;
            
            //переименовываем
            std::filesystem::rename(entry.path(), new_path);
            std::cout << old_name << " -> " << new_name << std::endl;
        }
    };
    
    //обрабатываем все файлы
    if (config.recursive) 
    {
        //ищем во всех подпапках
        for (const auto& entry : std::filesystem::recursive_directory_iterator(config.directory))
        {
            processFile(entry);
        }
    } 
    else 
    {
        //ищем только в этой папке
        for (const auto& entry : std::filesystem::directory_iterator(config.directory)) 
        {
            processFile(entry);
        }
    }
}

//функция для отображения использования
void showUsage(const std::string& program_name) 
{
    std::cout << "Использование:\n"
              << program_name << " <directory> <search_regex> <replace>\n"
              << program_name << " <directory> -r <search_regex> <replace>\n\n";
}

int main(int argc, char* argv[]) 
{
    try 
    {
        if (argc < 2) 
        {
            showUsage(argv[0]);
            return 1;
        }
        
        //вывод того, как правильно записывать агрументы
        std::string first_arg = argv[1];
        if (first_arg == "--help" || first_arg == "-h") 
        {
            showUsage(argv[0]);
            return 0;
        }
        
        //разбор аргументов командной строки
        Config config = parseArguments(argc, argv);
        
        //проверка существования и доступности директории
        if (!std::filesystem::is_directory(config.directory)) 
        {
            throw std::invalid_argument("no directory: " + config.directory.string());
        }
        
        //вывод информации о начале операции
        std::cout << "директория: " << config.directory << "\n";
        std::cout << "поиск: " << config.search_pattern << "\n";
        std::cout << "замена: " << config.replace_pattern << "\n";
        std::cout << "рекурсия: " << (config.recursive ? "yes" : "no") << "\n\n";
        
        //выполнение переименования
        performRename(config);
        
        std::cout << "\nоперация переименования завершена.\n";
        
    } 
    
    catch (const std::exception& e) 
    {
        //обработка всех исключений
        std::cerr << "ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}