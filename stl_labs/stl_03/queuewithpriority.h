#ifndef OOP_SEM4_QUEUEWITHPRIORITY_H
#define OOP_SEM4_QUEUEWITHPRIORITY_H

#include <list>
#include <string>
#include <iostream>

typedef enum
{
    LOW,
    NORMAL,
    HIGH } ElementPriority;

typedef struct
{
    std::string name;
} QueueElement;


class QueueWithPriority {
public:
    // Конструктор, создает пустую очередь
    QueueWithPriority();

    // Деструктор
    ~QueueWithPriority();

    // Добавить в очередь элемент element с приоритетом priority
    void PutElementToQueue(const QueueElement& element, ElementPriority priority);

    // Получить элемент из очереди
    // метод должен возвращать элемент с наибольшим приоритетом, который был
    // добавлен в очередь раньше других
    QueueElement GetElementFromQueue();

    // Выполнить акселерацию
    void Accelerate();
};


#endif //OOP_SEM4_QUEUEWITHPRIORITY_H
