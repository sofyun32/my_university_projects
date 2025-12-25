#ifndef TIMEMETR_H 
#define TIMEMETR_H

#include <memory>   // Для std::unique_ptr
#include <cstdint>  // Для int64_t

// Основной класс для измерения времени (Pimpl идиома)
class TimeMeter
{
private:
    class Impl;                  
    std::unique_ptr<Impl> pimpl;   

public:
    TimeMeter(unsigned int count); //count - кол-во меток
    ~TimeMeter();                   //необходим для unique_ptr

    // Запрещаем копирование (unique_ptr нельзя копировать)
    TimeMeter(const TimeMeter&) = delete;
    TimeMeter& operator=(const TimeMeter&) = delete;

    // Установить временную метку под номером num, вернуть время от старта в секундах
    double setTimeStamp(unsigned int num);
    
    // Получить метку num в секундах от старта
    double getSTimeStamp(unsigned int num) const;
    
    // Получить метку num в миллисекундах от старта
    int64_t getMSTimeStamp(unsigned int num) const;

    // Разница между двумя метками в секундах (second - first)
    double getsDiff(unsigned int first, unsigned int second) const;
    
    // Разница между двумя метками в миллисекундах (second - first)
    int64_t getMSDiff(unsigned int first, unsigned int second) const;

    // Проверка: разница между first и second меньше expected мс?
    bool isLess(unsigned int first, unsigned int second, int64_t expected) const;
    
    // Проверка: метка num от старта меньше expected мс?
    bool isLess(unsigned int num, int64_t expected) const;
};

#endif