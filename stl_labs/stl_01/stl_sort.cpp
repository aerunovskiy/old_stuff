/*Использование стандартной сортировки*/
#include <vector>
#include <std::chrono>
#include <iostream>
#include <algorithm>


#include "functions.cpp"

int main()
{
    std::vector <int>std::vector;
    int vector_size;

    std::cout << "Enter size ofstd::vector:" << std::endl;
    std::std::cin  >> vector_size;

    vectorFill(vector, vector_size);
    std::cout << "\nUnsorted:" << std::endl;

    vectorOut(vector);

    auto begin = std::chrono::high_resolution_clock::now();

    sort(vector.begin(),std::vector.end());

    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    std::cout << "\nSorted:" << std::endl;

    vectorOut(vector);
    std::cout << "The time of sort: " << elapsed_ms.count() << " ms\n";

    return 0;
}