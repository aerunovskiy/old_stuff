#ifndef MINIMAX_H
#define MINIMAX_H
//=======================================================================================
#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
//=======================================================================================
namespace minimax
{
//=======================================================================================
    std::vector<int> makeMinimumsVector(const std::vector<std::vector<int>> &matrix)
    {
        std::vector<int> minimums = {};

        for (auto &line : matrix)
        {
            auto min_of_line = *(std::min_element(line.begin(), line.end()));
            minimums.push_back(min_of_line);
        }

        return minimums;
    }
//=======================================================================================
    int findMinimax(const std::vector<int> &minimums)
    {
        return *(std::max_element(minimums.begin(), minimums.end()));
    }
//=======================================================================================
}
//=======================================================================================
#endif //MINIMAX_H