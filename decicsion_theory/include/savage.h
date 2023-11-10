#ifndef SAVAGE_H
#define SAVAGE_H
//=======================================================================================
namespace savage
{
//=======================================================================================
    std::vector<int> makeMaximumsOfColsVector(const std::vector<std::vector<int>> &matrix)
    {
        std::vector<int> maximums = {};

        auto col_count = matrix[0].size();

        for (size_t i = 0; i < col_count; ++i)
        {
            std::vector<int> column = {};

            for (auto  &line : matrix)
                column.push_back(line[i]);

            maximums.push_back(*std::max_element(column.begin(), column.end()));
        }

        return maximums;
    }
//=======================================================================================
    std::vector<std::vector<int>> makeSparseMatrix(const std::vector<std::vector<int>> &matrix,
                                                   const std::vector<int>              &maximums)
    {
        std::vector<std::vector<int>> sparse_matrix = matrix;

        for (auto &line : sparse_matrix)
        {
            for (size_t i = 0; i < line.size(); ++i)
                line[i] = maximums[i] - line[i];
        }

        return sparse_matrix;
    }
//=======================================================================================
    std::vector<int> makeMaximumsOfRowsVector(const std::vector<std::vector<int>> &matrix)
    {
        std::vector<int> maximums = {};

        for (auto &line : matrix)
        {
            auto max_of_line = *(std::max_element(line.begin(), line.end()));
            maximums.push_back(max_of_line);
        }

        return maximums;
    }
//=======================================================================================
    int findSavage(const std::vector<int> &sparse_maximums)
    {
        return *(std::min_element(sparse_maximums.begin(), sparse_maximums.end()));
    }
//=======================================================================================
}
//=======================================================================================
#endif //SAVAGE_H