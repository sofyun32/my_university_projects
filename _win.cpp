#include "TimeMeter.h"
#define WIN32_LEAN_AND_MEAN  // Исключаем редко используемые части Windows.h
#include <windows.h>         // Для QueryPerformanceCounter
#include <stdexcept>
#include <vector>

class TimeMeter::Impl
{
private:
    unsigned int count;           // Количество меток
    int64_t startTime;            // Время старта (в тиках)
    int64_t frequency;            // Частота таймера (тиков/сек)
    std::vector<int64_t> timeStamps; // Вектор меток (в тиках)

public:
    Impl(unsigned int count) : count(count), startTime(0), frequency(0) 
    {
        LARGE_INTEGER freq;
        // Получаем частоту таймера (неизменна в течение работы программы)
        if (!QueryPerformanceFrequency(&freq))
        {
            throw std::runtime_error("QueryPerformanceFrequency failed");
        }
        frequency = freq.QuadPart;
        
        LARGE_INTEGER start;
        // Получаем текущее значение счетчика как точку отсчета
        if (!QueryPerformanceCounter(&start))
        {
            throw std::runtime_error("QueryPerformanceCounter failed");
        }
        startTime = start.QuadPart;

        // Инициализируем вектор нулями
        timeStamps.resize(count, 0);
    }

    // Установка метки 
    double setTimeStamp(unsigned int num)
    {
        if (num >= count) throw std::out_of_range("Time stamp index out of range");

        LARGE_INTEGER current;
        if (!QueryPerformanceCounter(&current))
        {
            throw std::runtime_error("QueryPerformanceCounter failed");
        }

        timeStamps[num] = current.QuadPart;
        // Переводим тики в секунды: (current - start) / частота
        return static_cast<double>(current.QuadPart - startTime) / frequency;
    }

    // Получить метку в секундах от старта
    double getSTimeStamp(unsigned int num) const
    {
        if (num >= count) throw std::out_of_range("Time stamp index out of range");

        if (timeStamps[num] == 0)  // Проверка на установленную метку
        {
            throw std::runtime_error("Time stamp not set");
        }

        return static_cast<double>(timeStamps[num] - startTime) / frequency;
    }

    // Получить метку в миллисекундах от старта
    int64_t getMSTimeStamp(unsigned int num) const
    {
        if (num >= count) throw std::out_of_range("Time stamp index out of range");

        if (timeStamps[num] == 0)
        {
            throw std::runtime_error("Time stamp not set");
        }

        // (тики * 1000) / частота = миллисекунды
        return (timeStamps[num] - startTime) * 1000 / frequency;
    }

    // Разница между метками в секундах
    double getsDiff(unsigned int first, unsigned int second) const
    {
        return getSTimeStamp(second) - getSTimeStamp(first);
    }

    // Разница между метками в миллисекундах
    int64_t getMSDiff(unsigned int first, unsigned int second) const
    {
        return getMSTimeStamp(second) - getMSTimeStamp(first);
    }

    bool isLess(unsigned int first, unsigned int second, int64_t expected) const
    {
        return getMSDiff(first, second) < expected; 
    }

    bool isLess(unsigned int num, int64_t expected) const
    {
        return getMSTimeStamp(num) < expected;
    }
};

// Реализация методов TimeMeter 
TimeMeter::TimeMeter(unsigned int count) : pimpl(std::make_unique<Impl>(count)) {}
TimeMeter::~TimeMeter() = default;

double TimeMeter::setTimeStamp(unsigned int num) { return pimpl->setTimeStamp(num); }
double TimeMeter::getSTimeStamp(unsigned int num) const { return pimpl->getSTimeStamp(num); }
int64_t TimeMeter::getMSTimeStamp(unsigned int num) const { return pimpl->getMSTimeStamp(num); }

double TimeMeter::getsDiff(unsigned int first, unsigned int second) const { return pimpl->getsDiff(first, second); }
int64_t TimeMeter::getMSDiff(unsigned int first, unsigned int second) const { return pimpl->getMSDiff(first, second); }

bool TimeMeter::isLess(unsigned int first, unsigned int second, int64_t expected) const { return pimpl->isLess(first, second, expected); }
bool TimeMeter::isLess(unsigned int num, int64_t expected) const { return pimpl->isLess(num, expected); }