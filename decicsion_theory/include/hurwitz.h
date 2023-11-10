#ifndef HURWITZ_H
#define HURWITZ_H
//=======================================================================================
namespace hurwitz
{
//=======================================================================================
    const double C = 0.45;
//=======================================================================================
    std::vector<int> makeMaximumsVector(const std::vector<std::vector<int>> &matrix)
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
    std::vector<double> makeHurwitzVector(const std::vector<int> &maximums,
                                          const std::vector<int> &minimums)
    {
        std::vector<double> hurwitz_v = {};

        for (size_t i = 0; i < maximums.size(); ++i)
        {
            auto element = (maximums[i] * C) + (minimums[i] * (1 - C));
            hurwitz_v.push_back(element);
        }

        return hurwitz_v;
    }
//=======================================================================================
    double findHurwitz(const std::vector<double> &hurwitz_v)
    {
        return *(std::max_element(hurwitz_v.begin(), hurwitz_v.end()));
    }
//=======================================================================================
}
//=======================================================================================
#endif //HURWITZ_H