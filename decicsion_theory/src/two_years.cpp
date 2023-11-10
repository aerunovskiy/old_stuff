#include <iostream>
#include <vector>
#include <iomanip>

const int COST_FIRST_YEAR  = 25000;
const int COST_SECOND_YEAR = 10000;
const int COST_THIRD_YEAR  = 1000;
const int COST_PRICE       = 15000;

std::vector<std::vector<double>> makeFirstYearMatrix(std::vector<int> &demand)
{
    std::vector<std::vector<double>> matrix = {};

    int max_order_cost = 0;

    for (size_t i = 0; i < demand.size(); ++i)
    {
        std::vector<double> row = {};

        for (size_t j = 0; j < demand.size(); ++j)
        {
            auto result = COST_FIRST_YEAR * demand[j] -  COST_PRICE * demand[i];

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

std::vector<std::vector<double>> makeSecondAndThirdYearMatrix(std::vector<int> &demand)
{
    std::vector<std::vector<double>> matrix = {};

    int max_order_cost = 0;

    for (size_t i = 0; i < demand.size(); ++i)
    {
        std::vector<double> row = {};

        for (size_t j = 0; j < demand.size(); ++j)
        {
            int result = 0;

            if (demand[i] < demand[j])
                result = demand[j] * COST_THIRD_YEAR;
            else
                result = demand[j] * COST_SECOND_YEAR + (demand[i] - demand[j]) * COST_THIRD_YEAR;

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

template <typename T>
void printVector(const std::vector<T> &minimums)
{
    for (auto &min : minimums)
        std::cout << std::setw(3) << min << ' ';

    std::cout << std::endl;
}

std::vector<double> makeSumVector(const std::vector<std::vector<double>> &matrix,
                                  const std::vector<double>              &probability)
{
    std::vector<double> sum_v = {};

    for (auto &row : matrix)
    {
        double sum = 0;
        auto pr_it = probability.begin();

        for (auto &num : row)
            sum += *pr_it++ * num;

        sum_v.push_back(sum);
    }


    return sum_v;
}
//=======================================================================================
template <typename T, typename P>
void printMatrix(const std::vector<std::vector<T>> &matrix,
                 const std::vector<P>              &demand)
{
    auto d_it = demand.begin();
    for (auto &line : matrix)
    {
        std::cout << std::setw(3) << *d_it++ << '|';

        for (auto &number : line)
            std::cout  << std::setw(10) << std::fixed << std::setprecision(1) << number << ' ';

        std::cout << std::endl;
    }
}

std::vector<double> countR(const std::vector<double> &sum1,
                           const std::vector<double> &sum2,
                           const std::vector<double> &probability)
{
    std::vector<double> r_v = {};

    for (size_t i = 0; i < sum1.size(); ++i)
    {
        double r = 0;

        for (size_t j = 0; j < i; ++j)
            r += probability[i] * sum2[(i - 1) - j + 1];

        r_v.push_back(sum1[i] + r);
    }

    return r_v;
}

void printR(const std::vector<double> &r_v)
{
    for (size_t i = 0; i < r_v.size(); ++i)
        std::cout << "R" << i + 1 << " = " << r_v[i] << std::endl;
}
//=======================================================================================
double getMaxR(const std::vector<double> &r_v)
{
    return *(std::max_element(r_v.begin(), r_v.end()));
}
//=======================================================================================
int findRIndex(const std::vector<double> &r_v)
{
    auto max = getMaxR(r_v);

    for (size_t i = 0; i < r_v.size(); ++i)
    {
        if (r_v[i] == max)
            return i;
    }

    return -1;
}
//=======================================================================================
int main()
{
    std::vector<double> probability1 = {  0.4, 0.25, 0.15,  0.1, 0.05, 0.03, 0.02 };
    std::vector<double> probability2 = { 0.01, 0.03, 0.06, 0.15,  0.2, 0.25,  0.3 };
    std::vector<int>    demand       = {    0,   50,  100,  150,  200,  250,  300 };

    auto fy_matrix  = makeFirstYearMatrix         (demand);
    auto sty_matrix = makeSecondAndThirdYearMatrix(demand);

    // Вывод исходных данных
    std::cout << "Цена в 1 год: "  << COST_FIRST_YEAR  << std::endl;
    std::cout << "Цена во 2 год: " << COST_SECOND_YEAR << std::endl;
    std::cout << "Цена в 3 год: "  << COST_THIRD_YEAR  << std::endl;
    std::cout << "Себестоимость: " << COST_PRICE       << std::endl;

    std::cout << "Спрос:" << std::endl;
    printVector(demand);

    std::cout << "Вероятность что не купят в первый год:" << std::endl;
    printVector(probability1);

    std::cout << "Вероятность что не купят во второй год:" << std::endl;
    printVector(probability2);

    auto sum1 = makeSumVector(fy_matrix,  probability1);
    auto sum2 = makeSumVector(sty_matrix, probability2);

    std::cout << "Таблица доходов за 1 год:" << std::endl;
    printMatrix(fy_matrix, demand);

    std::cout << "Таблица доходов за 2/3 год:" << std::endl;
    printMatrix(sty_matrix, demand);

    auto r_v = countR(sum1, sum2, probability1);
    printR(r_v);

    auto demand_index = findRIndex(r_v);
    std::cout << "Заказать нужно " << demand[demand_index] << " автомобилей." << std::endl;

    auto max_r = getMaxR(r_v);
    if (max_r > 0)
        std::cout << "Прибыль: " << max_r << std::endl;
    else
        std::cout << "Минимальная убыль:" << max_r << std::endl;

    return 0;
}

