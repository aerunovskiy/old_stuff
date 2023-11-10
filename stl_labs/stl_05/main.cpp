//region Task
/*
 * Выполнить ВСЕ задания
 * Написать программу, которая выполняет следующие действия:
 *  1. Заполняет vector<DataStruct> структурами DataStruct, при этом key1 и key2,
 *     генерируются случайным образом в диапазоне от -5 до 5, str заполняется из таблицы
 *     (таблица содержит 10 произвольных строк, индекс строки генерируется случайным образом)
 *  2. Выводит полученный вектор на печать
 *  3. Сортирует вектор следующим образом:
 *      1. По возрастанию key1
 *      2. Если key1 одинаковые, то по возрастанию key2
 *      3. Если key1 и key2 одинаковые, то по возрастанию длинны строки str
 *      4. Выводит полученный вектор на печать
 *
 * DataStruct определена следующим образом:
 *  typedef struct
 *  {
 *   int       key1;
 *   int       key2;
 *   string  str;
 *  } DataStruct;
 * */
//endregion Task

//region Include
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
//endregion Include

struct DataStruct
{
    int key1;
    int key2;
    std::string  str;
} ;

DataStruct NewStruct(std::vector<std::string>& str)
{
    int sign = rand () % 2;
    int key1 = (sign == 1) ? (rand() % 5) : (rand() % 5) * -1;
    sign = rand () % 2;
    int key2 = (sign == 1) ? (rand() % 5) : (rand() % 5) * -1;

    DataStruct newStruct = {key1, key2, str[rand() % 10]};
    return newStruct;
}

void Print(std::vector<DataStruct>& v)
{
    for (auto& i : v)
        std::cout << std::setw(2) << i.key1 << " " << std::setw(2) << i.key2 << " " << i.str << std::endl;
}

void FillTable(std::vector<std::string>& t)
{
    t.emplace_back("First string");
    t.emplace_back("Second string");
    t.emplace_back("Third string");
    t.emplace_back("Fourth string");
    t.emplace_back("Fifth string");
    t.emplace_back("Sixth string");
    t.emplace_back("Seventh string");
    t.emplace_back("Eighth string");
    t.emplace_back("Ninth string");
    t.emplace_back("Tenth string");
}

void Sort( std::vector<DataStruct>& ds)
{
    sort(ds.begin(), ds.end(), [](const DataStruct& ds1, const DataStruct& ds2) -> bool
         {
            if (ds1.key1 < ds2.key1)
                return true;
            else if (ds1.key2 < ds2.key2)
                return true;
            else if ((ds1.key1 == ds2.key1)&&(ds1.key2 == ds2.key2))
                return ds1.str.length() < ds2.str.length();
         }
    );

}

int main ()
{
    std::vector<DataStruct> structs;
    std::vector<std::string> table;

    FillTable(table);

    int i = 10;
    while (i)
    {
        structs.push_back(NewStruct(table));
        --i;
    }

    std::cout << "Before sort:\n";

    Print(structs);

    /*sort(structs.begin(), structs.end(), [](const DataStruct& ds1, const DataStruct& ds2) -> bool
    {
        return ds1.key1 < ds2.key1;
    }
    );*/

    Sort(structs);

    std::cout << "After sort:\n";

    Print(structs);

    return 0;
}