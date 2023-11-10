//region Task
/*
 * Написать программу, которая выполняет следующие действия:
 *  a. Читает содержимое текстового файла
 *  b. Выделяет слова, словом считаются последовательность символов разделенных пробелами
 *     и/или знаками табуляции и/или символами новой строки
 *  c. Вывести список слов присутствующий в тексте без повторений (имеется в виду, что одно
 *     и то же слово может присутствовать в списке только один раз)
*/
//endregion Task

//region Include
#include <iostream>
#include <algorithm>
#include <fstream>
#include <map>
//endregion Include

void WordCount(const std::map<std::string, int>::value_type &vt)
{
    if (vt.second == 1)
        std::cout << vt.first << std::endl;
}

int main()
{
    std::ifstream fInput ("../stl_06/erunovskiy/input.txt");
    std::map<std::string, int> myMap;
    std::string str;

    while(!fInput.eof() && fInput >> str)
        ++myMap[str];
    for_each(++myMap.begin(), ++myMap.end(), WordCount);
}