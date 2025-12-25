#include "TimeMeter.h"
#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

//Функция для измерения времени выполнения любой функции
template<typename Func, typename... Args>
double measureExecutionTime(Func&& func, Args&&... args) 
{
    TimeMeter tm(2); // Создаем TimeMeter с двумя метками
    
    tm.setTimeStamp(0); // Метка начала
    
    // Вызываем переданную функцию с аргументами
    std::forward<Func>(func)(std::forward<Args>(args)...);
    
    tm.setTimeStamp(1); // Метка конца
    
    // Возвращаем разницу во времени в секундах
    return tm.getsDiff(0, 1);
}

// Пример функции для тестирования
int multiply(int a, int b) 
{
    // Имитация работы
    volatile int result = 0; //volatile предотвращает оптимизацию компилятором (чтобы цикл не был удален)
    for (int i = 0; i < 1000000; ++i) 
    {
        result = a * b;
    }
    return result;
}

void powerVectorElements(std::vector<int>& vec, int power) 
{
    // Создаем функтор для возведения в степень с помощью std::bind 
    auto powerFunc = std::bind(
        [](int x, int power) 
        {
            int result = 1;
            for (int i = 0; i < power; ++i) 
            {
                result *= x;
            }
            return result;
        },
        std::placeholders::_1,
        power
    );
    
    // Применяем преобразование к каждому элементу вектора
    std::transform(vec.begin(), vec.end(), vec.begin(), powerFunc);
}

int main() 
{
    // Часть 1: Измерение времени выполнения функций
    
    std::cout << "=== Часть 1: Измерение времени выполнения функций ===\n";
    
    // 1.1 Измерение времени выполнения лямбда-функции
    auto lambda = []() 
    {
        volatile double sum = 0;
        for (int i = 0; i < 1000000; ++i) 
        {
            sum += i * 0.1;
        }
        return sum;
    };
    
    double lambdaTime = measureExecutionTime(lambda);
    std::cout << "Лямбда-функция выполнена за: " << lambdaTime * 1000 << " мс\n";
    
    // 1.2 Измерение времени выполнения функции с помощью std::bind
    auto boundFunc = std::bind(multiply, 5, 7);
    double bindTime = measureExecutionTime(boundFunc);
    std::cout << "Функция через std::bind выполнена за: " << bindTime * 1000 << " мс\n";
    
    // 1.3 Измерение времени выполнения обычной функции с аргументами
    double funcTime = measureExecutionTime(multiply, 10, 20);
    std::cout << "Обычная функция выполнена за: " << funcTime * 1000 << " мс\n";
    
    std::cout << "\n=== Часть 2: Возведение в степень элементов вектора ===\n";
    
    // Создаем вектор чисел
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    std::cout << "Исходный вектор: ";
    for (int num : numbers) 
    {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    
    // Возводим все элементы в квадрат (степень 2)
    powerVectorElements(numbers, 2);
    std::cout << "После возведения в квадрат: ";
    for (int num : numbers) 
    {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    
    return 0;
}