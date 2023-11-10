/*
 * Прочитайте во встроенный массив С содержимое текстового файла, скопируйте данные в вектор
 * одной строкой кода (без циклов и алгоритмов STL)
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int main ()
{
    std::vector <std::string> string_vector;
    setlocale(LC_ALL, "RUS");
    std::string str;

    std::ifstream fInput ("../stl_01/text.txt");

    if (fInput.is_open())
    {
        std::stringstream ss;
        ss << fInput.rdbuf();
        str = ss.str();
    }
    fInput.close();

    string_vector.push_back(str);

    for (int i = 0; i < string_vector.size(); ++i)
        std::cout << string_vector.at(i);

    return 0;
}