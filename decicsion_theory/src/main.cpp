#include "minimax.h"
#include "savage.h"
#include "hurwitz.h"
//=======================================================================================
void printMatrix(const std::vector<std::vector<int>> &matrix)
{
    for (auto &line : matrix)
    {
        for (auto &number : line)
            std::cout << std::setw(3) << number << ' ';

        std::cout << std::endl;
    }
}
//=======================================================================================
template <typename T>
void printVector(const std::vector<T> &minimums)
{
    for (auto &min : minimums)
        std::cout << std::setw(3) << min << ' ';

    std::cout << std::endl;
}
//=======================================================================================
int main()
{
    std::vector< std::vector<int> > matrix = {{ 15,  10,   0,  -6,  17 },
                                              {  3,  14,   8,   9,   2 },
                                              {  1,   5,  14,  20,  -3 },
                                              {  7,  19,  10,   2,   0 }};

    std::cout << "=============================== MINIMAX ===============================" << std::endl;
    {
        std::cout << "Исходная матрица:"    << std::endl;
        printMatrix(matrix);

        auto minimums = minimax::makeMinimumsVector(matrix);
        std::cout << "Минимальные значения по строкам:" << std::endl;
        printVector(minimums);

        std::cout << "По критерию Минимакса оптимальным решением является: "
                  << minimax::findMinimax(minimums) << std::endl;

    }

    std::cout << "================================ SAVAGE ===============================" << std::endl;
    {
        std::cout << "Исходная матрица:"    << std::endl;
        printMatrix(matrix);

        std::cout << "Мaксимальные значения по столбцам:" << std::endl;
        auto maximums = savage::makeMaximumsOfColsVector(matrix);
        printVector(maximums);

        auto sparse_matrix = savage::makeSparseMatrix(matrix, maximums);
        std::cout << "Матрица потерь:"    << std::endl;
        printMatrix(sparse_matrix);

        std::cout << "Мaксимальные значения по строкам матрицы потерь:" << std::endl;
        auto sparse_maximums = savage::makeMaximumsOfRowsVector(sparse_matrix);
        printVector(sparse_maximums);

        std::cout << "По критерию Сэвиджа оптимальным решением является: "
                  << savage::findSavage(sparse_maximums) << std::endl;
    }

    std::cout << "================================ HURWITZ ===============================" << std::endl;

    {
        std::cout << "Исходная матрица:"    << std::endl;
        printMatrix(matrix);

        auto maximums = hurwitz::makeMaximumsVector(matrix);
        std::cout << "Макисмальные значения по строкам:" << std::endl;
        printVector(maximums);

        auto minimums = hurwitz::makeMinimumsVector(matrix);
        std::cout << "Минимальные значения по строкам:" << std::endl;
        printVector(minimums);

        std::cout << "Массив взвешенных средних:" << std::endl;
        auto hurwitz_v = hurwitz::makeHurwitzVector(maximums, minimums);
        printVector(hurwitz_v);

        std::cout << "По критерию Гурвица оптимальным решением является: "
                  << hurwitz::findHurwitz(hurwitz_v) << std::endl;
    }

    return 0;
}
//=======================================================================================