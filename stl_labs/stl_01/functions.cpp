#include <iomanip>
#include <fstream>

template <typename T>
void bubbleSort(std::vector <T> &vec)
{
    T temp;

    for (int i = 0; i < vec.size() - 1; ++i)
    {
        for (int j = 0; j < vec.size() - i - 1; ++j)
        {
            if (vec[j] > vec[j + 1])
            {
                temp       = vec[j];
                vec[j]     = vec[j + 1];
                vec[j + 1] = temp;
            }
        }
    }
}

template <typename T>
void bubbleSortAt(std::vector <T> &vec)
{
    T temp;

    for (int i = 0; i < vec.size() - 1; i++)
    {
        for (int j = 0; j < vec.size() - i - 1; j++)
        {
            if (vec.at(j) > vec.at(j + 1))
            {
                temp          = vec.at(j);
                vec.at(j)     = vec.at(j + 1);
                vec.at(j + 1) = temp;
            }
        }
    }
}

template <typename T>
void bubbleSortIterator(std::vector <T> &v)
{
    auto first  = v.begin();
    auto second = first;

    T temp;

    for (first; first < v.end() - 1; first++)
    {
        for (second = first+1; second < v.end(); second++)
        {
            if (*first > *second)
            {
                temp    = *second;
                *second = *first;
                *first  = temp;
            }
        }
    }
}

template <typename T>
void vectorOut(std::vector <T>& vec)
{
    int i = 1;

    for (const T &x : vec)
    {
        std::cout << std::setw(3) << x << " ";

        if (i++ % 10 == 0)
            std::cout << std::endl;
    }
}

template <typename T>
void vectorFill(std::vector <T> & vec, int SIZE)
{
    srand((unsigned)time(nullptr));

    for (int i = 0; i < SIZE; ++i)
    {
        vec.push_back(rand() % 100);
    }
}

void vectorFill(std::vector <double>& vec, int SIZE)
{
    int sign;
    srand((unsigned)time(nullptr));
    for (int i = 0; i < SIZE; ++i)
    {
        sign = (rand() % 2 == 1) ? 1 : -1;

        vec.push_back(rand() % 100 / 100.0 * sign);
    }
}

template <typename T>
void fillVectorFromFile (std::vector <T> & vec)
{
    std::ifstream fout;

    fout.open("../result.txt");

    copy(vec.begin(),vec.end());

    fout.close();

    std::cout << "End.\n";
}