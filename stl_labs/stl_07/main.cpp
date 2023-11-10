//region Task
/* Разработать функтор, позволяющий собирать статистику о последовательности целых чисел
 * (послед может содержать числа от -500 до 500).Функтор после обработки последовательности
 * алгоритмом for_each должен предоставлять следующую статистику:
 *      a. Максимальное число в последовательности
 *      b. Минимальное число в последовательности
 *      c. Среднее число в последовательности
 *      d. Количество положительных чисел
 *      e. Количество отрицательных чисел
 *      f. Сумму нечетных элементов последовательности
 *      g. Сумму четных элементов последовательности
 *      h. Совпадают ли первый и последний элементы последовательности.
 * Проверить работу программы на случайно сгенерированных последовательностях.
 * */
//endregion Task

//region Include
#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>
//endregion Include

void FillVector(std::vector<int>& v)
{
    int sign = 0;
    int newNumber = 0;

    srand(time(NULL));

    for (int i = 1000; i > 0; --i)
    {
        sign = rand() % 2;
        newNumber = (sign == 1) ? (rand() % 501) : (rand() % 501) * -1;
        v.push_back(newNumber);
    }
}

void PrintVector(const std::vector<int>& v)
{
    int i = 1;
    for (auto item : v) {
        std::cout << std::setw(4) << item << " ";
        if (i++ % 20 == 0)
            std::cout << std::endl;
    }
}

class MyFunctor
{
    int _even;
    int _odd;
    int _max;
    int _min;
    int _positive;
    int _negative;
    int _first;
    int _last;
    int _counter;
public:
   // MyFunctor(const vector<int>& v) :
   MyFunctor() : _even(0), _odd(0), _max(0), _min(0), _positive(0), _negative(0),
   _first(0), _last(0), _counter(0){}
   void operator()(int x)
   {
       if (_counter == 0)
           _first = x;

       if (_counter == 999)
           _last = x;

       if (_max < x)
           _max = x;

       if (_min > x)
           _min = x;

       if (x % 2 == 0)
           _even += x;
       else
           _odd += x;

       if (x > 0)
           _positive++;
       else if (x < 0)
           _negative++;

       _counter++;
   }
   int even_sum() const { return _even; };
   int odd_sum()  const { return _odd;  };
   double average() const { return (even_sum() + odd_sum())/1000; }
   int max() const { return  _max; };
   int min() const { return  _min; };
   int positive() const { return _positive; };
   int negative() const { return _negative; };
   bool isEqual() const { return _first == _last; };
};

int main ()
{
    std::vector <int> sequence;
    MyFunctor myFunctor;

    FillVector(sequence);

    PrintVector(sequence);

   // sequence[sequence.size()-1] = sequence[0];

    myFunctor = for_each(sequence.begin(), sequence.end(), myFunctor);

    std::cout << "Even sum: " << myFunctor.even_sum() << std::endl;
    std::cout << "Odd sum: "  << myFunctor.odd_sum()  << std::endl;
    std::cout << "Average: " << myFunctor.average() << std::endl;
    std::cout << "Max number: " << myFunctor.max() << std::endl;
    std::cout << "Min number: " << myFunctor.min() << std::endl;
    std::cout << "Positive: " << myFunctor.positive() << std::endl;
    std::cout << "Negative: " << myFunctor.negative() << std::endl;
    sequence[sequence.size()-1] = sequence[0];
    std::cout << "First: " << sequence[0] << " Last: " << sequence[sequence.size()-1] << std::endl;
    std::cout << "First and last is equal? " << myFunctor.isEqual() << std::endl;

    return 0;
}

