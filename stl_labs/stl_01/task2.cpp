/*
 * Напишите алгоритм сортировки (любой простейший) содержимого вектора целых чисел,
 * используя метод at().
 */

#include <vector>
#include <chrono>
#include <iostream>

#include "functions.cpp"

int main()
{
    std::vector<int> vector;
    int vector_size;

    std::cout << "Enter size ofstd::vector:" << std::endl;
    std::cin  >> vector_size;

    vectorFill(vector, vector_size);
    std::cout << "\nUnsorted:" << std::endl;

    vectorOut(vector);

    auto begin = std::chrono::high_resolution_clock::now();

    bubbleSortAt(vector);

    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    std::cout << "\nSorted:" << std::endl;

    vectorOut(vector);
    std::cout << "The time of sort: " << elapsed_ms.count() << " ms\n";

    return 0;
}