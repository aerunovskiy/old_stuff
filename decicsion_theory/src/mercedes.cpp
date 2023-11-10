#include <iostream>
#include <vector>
#include <iomanip>
//=======================================================================================
const int COST_END_OF_YEAR = 15;
const int COST_START       = 49;
const int COST_PRICE       = 25;
//=======================================================================================
template <typename T, typename P>
void printMatrix(const std::vector<std::vector<T>> &matrix,
                 const std::vector<P>              &demand)
{
    auto d_it = demand.begin();
    for (auto &line : matrix)
    {
        std::cout << *d_it++ << '|';

        for (auto &number : line)
            std::cout << std::setw(5) << number << ' ';

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
std::vector<std::vector<double>> makeMatrix(const std::vector<int>    &demand,
                                            const std::vector<double> &probability)
{
    std::vector<std::vector<double>> matrix = {};

    int max_order_cost = 0;

    for (size_t i = 0; i < demand.size(); ++i)
    {
        std::vector<double> row = {};

        for (size_t j = 0; j < demand.size(); ++j)
        {
            auto result = COST_START       * demand[j] -
                          COST_PRICE       * demand[i] +
                          COST_END_OF_YEAR * (demand[i] - demand[j]);

            if (demand[j] == demand[i])
            {
                row.push_back(result);
                max_order_cost = result;
            }

            if (demand[j] > demand[i])
                row.push_back(max_order_cost);
            else
                row.push_back(result);
        }

        matrix.push_back(row);
    }

    return matrix;
}
//=======================================================================================
std::vector<double> countProfit(const std::vector<std::vector<double>> &matrix,
                                const std::vector<double>              &probability)
{
    std::vector<double> profit_v = {};


    for (auto &row : matrix)
    {
        double profit = 0;
        auto   p_it   = probability.begin();

        for (auto &num : row)
            profit += num * *p_it++;

        profit_v.push_back(profit);
    }

    return profit_v;
}
//=======================================================================================
double getMaxProfit(const std::vector<double> &profit_v)
{
    return *(std::max_element(profit_v.begin(), profit_v.end()));
}
//=======================================================================================
int findProfitIndex(const std::vector<double> &profit_v)
{
    auto max = getMaxProfit(profit_v);

    for (size_t i = 0; i < profit_v.size(); ++i)
    {
        if (profit_v[i] == max)
            return i;
    }

    return -1;
}
//=======================================================================================
int main()
{


    std::vector<int>    demand      = {  100, 150, 200, 250,  300 };
    std::vector<double> probability = { 0.25, 0.3, 0.1, 0.1, 0.25 };

    auto  matrix = makeMatrix(demand, probability);
    std::cout << "Матрица спроса и заказа:" << std::endl;
    printMatrix(matrix, demand);

    auto profit_v = countProfit(matrix, probability);
    std::cout << "Ожидаемая прибыль:" << std::endl;
    printVector(profit_v);

    auto max_profit = getMaxProfit(profit_v);
    std::cout << "Максимальная прибыль:" << max_profit << std::endl;

    std::cout << "Заказать нужно " << demand[findProfitIndex(profit_v)] << " Мерседесов." << std::endl;




    return 0;
}
//=======================================================================================