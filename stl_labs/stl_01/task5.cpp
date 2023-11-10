/*
 * Напишите программу, сохраняющую в векторе числа, полученные из стандартного ввода
 * (окончанием ввода является число 0). Удалите все элементы, которые делятся на 2
 * (не используете стандартные алгоритмы STL), если последнее число 1. Если последнее
 * число 2, добавьте после каждого числа которое делится на 3 три единицы.
 */

#include <iostream>
#include <vector>

void eraseIf1(std::vector<int> &vec)
{
    for (int i = 0; i < vec.size();)
    {
        if (vec[i] % 2 == 0)
            vec.erase(vec.begin() + i);
        else
            ++i;
    }
}

void insertIf2(std::vector<int> &vec)
{
    auto first  = vec.begin();
    auto second = vec.end();

    while (first < second)
    {
        if (*first % 3 == 0)
        {
            vec.insert(++first, 111);
            second++;
        }

        first++;
    }
}

int main ()
{
    std::vector <int> vector;
    int number = 0;

    while (number)
    {
        std::cout << "\nEnter a number (0 is exit)\n";
        std::cin >> number;
        vector.push_back(number);
    }

    vector.pop_back();

    auto num = vector.at(vector.size()-1);

    switch (num)
    {
        case 1:
        {
            eraseIf1(vector);
            break;
        }
        case 2:
        {
            insertIf2(vector);
            break;
        }
        default:
        {
            std::cout << "Wrong number!\n";
            break;
        }

    }

    for (const int i : vector)
        std::cout << i;

    return 0;
}