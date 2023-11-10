#include <iostream>
#include <string>
#include <algorithm>

#define WORD_LENGTH    10;
#define STRING_LENGHT  40;

size_t unicode_length(const std::string &text) {
    size_t length = 0;

    for (auto p = text.c_str(); *p != 0; ++p)
        length += ((*p & 0xc0) != 0x80);

    return length;
}

bool IsExtraSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }

void EraseExtraSpaces(std::string &str)
{
    std::string::iterator new_end = unique(str.begin(), str.end(), IsExtraSpaces);
    str.erase(new_end, str.end());
}


void WrongSpaces (std::string &str)
{
    int currentPosition = str.find_first_of(",.?!:;");

    while (currentPosition != std::string::npos)
    {
        if ((int)str[currentPosition - 1] == 32)
        {
            str.erase(currentPosition - 1, 1);
            currentPosition--;
        }

        if ((int)str[currentPosition + 1] != 32)
        {
            str.insert(currentPosition + 1, " ");
        }
        currentPosition = str.find_first_of(",.?!:;", currentPosition + 1);
    }
}

void ReplaceLongWords(std::string &str, const std::string &vau, size_t maxLength) {
    int end = str.find_first_of(".,:;?! ");
    int begin = 0;
    int offset = maxLength - vau.size();

    while (end != std::string::npos and begin != std::string::npos) {
        std::string tmp(str, begin, (end - begin));
        if (unicode_length(tmp) > maxLength) {
            str.replace(begin, end - begin, vau);
            end -= offset;
        }
        begin = str.find_first_not_of(".,:;?! ", end); //posMark+1
        end = str.find_first_of(".,:;?! ", begin + 1);
    }
}

void StringOut(const std::string &toPrint, int outputMaxLength) {

    if (unicode_length(toPrint) <= outputMaxLength) {
        std::cout << toPrint << std::endl;
        return;
    }

    std::vector<std::string> printMe(0);
    int begin = 0;
    int posMark = toPrint.find(' ');
    int cutMark = posMark;

    while (posMark != std::string::npos and begin != std::string::npos)
    {
        std::string tmp(toPrint, begin, (posMark - begin));
        if (unicode_length(tmp) > outputMaxLength) {
            std::string tmp(toPrint, begin, cutMark - begin);
            if (tmp[0] == 32){
                tmp.erase(0,1);
            }
            printMe.push_back(tmp);
            begin = cutMark + 1;
        }
        cutMark = posMark;
        posMark = toPrint.find(' ', posMark + 1);
    }

    if (cutMark != toPrint.size())
    {
        std::string tmp(toPrint, begin, std::string::npos);
        printMe.push_back(tmp);
    }

    for (const auto &i : printMe)
        std::cout << i << std::endl;
}




