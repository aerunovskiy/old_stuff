#include <iostream>
#include <list>
#include <vector>
#include "queuewithpriority.h"

void fillList(std::list<int> &list, size_t length);
void printList(std::list<int> &list);
void printListSpec(std::list<int> &list);

int main()
{
  //  QueueWithPriority q = QueueWithPriority();
    std::list <int>  myList;
    std::vector<int> listLengths = {0, 1, 2, 3, 4, 5, 7, 14};

    for (auto listLength : listLengths)
    {
        fillList(myList, static_cast<size_t>(listLength));

        if (myList.empty()) continue;
    }

    printList(myList);
    printListSpec(myList);




    return 0;
}

void fillList(std::list<int> &list, size_t length)
{
    while (length--)
        list.push_back(1 + rand() % 20);
}

void printList(std::list<int> &list)
{
    for (auto element : list)
        std::cout << element << " ";

    std::cout << std::endl;
}

void printListSpec(std::list<int> &list)
{
    for (auto element : list)
        std::cout << element << " ";

    std::cout << std::endl;
}