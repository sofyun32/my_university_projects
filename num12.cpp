#include <iostream>     
#include <thread>        // Для работы с потоками
#include <chrono>        // Для работы со временем (задержки)
#include <atomic>        // Для потокобезопасных переменных
#include <string>       

// Функция для отображения прогресс-бара в консоли
void showProgress(std::atomic<int>& progress) 
{
    while (progress < 100) 
    {
        std::cout << "\rПрогресс: [";
       
        int bars = progress / 2;
        
        for (int i = 0; i < 50; i++) 
        {
            std::cout << (i < bars ? "=" : " ");
        }
        
        std::cout << "] " << progress << "%";
        std::cout.flush();  // Принудительно выводим данные в консоль
        
        // Задержка между обновлениями (100 мс = 10 раз в секунду)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Функция, выполняющаяся в отдельном потоке (имитация работы)
void backgroundTask(std::atomic<int>& progress) 
{
    for (int i = 0; i <= 100; i++) 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        // Атомарно обновляем значение прогресса
        progress = i;
    }
}

int main() 
{
    // Создаем атомарную переменную для хранения прогресса
    std::atomic<int> progress{0};
    
    // Выводим инструкции пользователю
    std::cout << "=== Демонстрация прогресса в отдельном потоке ===\n";
    std::cout << "Поток 1: выполняет работу (backgroundTask)\n";
    std::cout << "Поток 2: отображает прогресс (showProgress)\n";
    std::cout << "Нажмите Enter для старта...";
    std::cin.get();  // Ждем нажатия Enter
    
    // Запускаем фоновую задачу в отдельном потоке
    std::thread taskThread(backgroundTask, std::ref(progress));
    
    // Вызываем функцию отображения прогресса в основном потоке
    // Эта функция будет работать параллельно с taskThread
    showProgress(progress);

    // Ждем завершения фонового потока
    // join() блокирует текущий поток, пока taskThread не завершится
    taskThread.join();

    progress = 0;
    std::thread taskThread2(backgroundTask, std::ref(progress));
    showProgress(progress);
    taskThread2.join();

    std::cout << "\n\n=== Задача успешно выполнена! ===\n";
    
    return 0;  
}
