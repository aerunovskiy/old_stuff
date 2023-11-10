#include <iostream>
#include <vector>

#include "functions.cpp"

int main()
{
    std::vector <double> vector;
    int vector_size = 0;

    std::cout << "Enter size of vector:" << std::endl;
    std::cin >> vector_size;

    vectorFill(vector, vector_size);

    std::cout << "\nUnsorted:" << std::endl;

    vectorOut(vector);

    bubbleSort(vector);

    std::cout << "\nSorted:" << std::endl;

    vectorOut(vector);

    return 0;
}