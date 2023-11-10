//region Task
/* Разработать программу, которая, используя только стандартные алгоритмы и функторы,
 * умножает каждый элемент списка чисел с плавающей  точкой на число PI
 */
//endregion Task

//region Include
#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>
//endregion Include

void FillVector(std::vector <float>& v)
{
    int sign = 0;
    float newNumber = 0;

    srand(time(NULL));

    for (int i = 1000; i > 0; --i)
    {
        sign = rand() % 2;
        newNumber = (sign == 1) ? (float(rand())/float((RAND_MAX)) * 501.0 ) : (float(rand())/float((RAND_MAX)) * 501.0 ) * -1;
        v.push_back(newNumber);
    }
}

void PrintVector(const std::vector<float>& v)
{
    int i = 1;
    for (auto item : v) {
        std::cout << std::setw(8) << item << " ";
        if (i++ % 14 == 0)
            std::cout << std::endl;
    }
}

class MyFunctor
{
public:
    MyFunctor()  {}
    void operator()(float& x)
    {
        x *= 3.14;
    }
};

int main ()
{
    std::vector<float> floatVector;
    MyFunctor myFunctor;

    FillVector(floatVector);

    PrintVector(floatVector);

    //myFunctor = for_each(floatVector.begin(), floatVector.end(), myFunctor);
    for_each(floatVector.begin(), floatVector.end(), myFunctor);

    std::cout << "\nAfter multiplication on PI: \n ";

    PrintVector(floatVector);

    return 0;
}