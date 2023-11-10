//region Task
/* Напишите программу – «телефонную книжку».
 * Записи (имя и телефон) должны хранится в каком-либо STL-контейнере (vector или list),
 * причем крайне желательно, чтобы от типа контейнера не зависело ничего, кроме одной строки
 * в программе – объявления этого контейнера (указание: используйте typedef).
 *
 * Программа должна поддерживать следующие операции:
 * • Просмотр текущей записи
 * • Переход к следующей записи
 * • Переход к предыдущей записи
 * • Вставка записи перед/после просматриваемой
 * • Замена просматриваемой записи
 * • Вставка записи в конец базы данных
 * • Переход вперед/назад через n записей.
 *
 * Помните, что после вставки элемента итераторы становятся недействительными,
 * поэтому после вставки целесообразно переставлять итератор на начало базы данных.
 * Постарайтесь реализовать операции вставки и замены с помощью одной и той же функции,
 * которой в зависимости от требуемого действия передается либо обычный итератор, либо
 * адаптер – один из итераторов вставки: void modifyRecord(iterator pCurrentRecord, CRecord newRecord).
 * Программа может сразу после запуска сама (без команд пользователя) заполнить
 * записную книжку некоторым количеством записей.
 * */

//endregion Task

#include <iostream>
#include <list>
#include "Contacts.h"

Contacts *contacts = new Contacts();

void PrintContactsList(std::list <Contacts>& contactsList);

int main()
{
    std::list <Contacts> contactsList;

    contacts->FirstInit(contactsList);

    PrintContactsList(contactsList);

    auto it = contactsList.begin();

    uint key = 0;

    contacts->PrintMenu();

    //region WorkCycle
    while (key != 7)
    {
        std::cin >> key;

        switch (key)
        {
            case 1 :
                contacts->PrintItem(*it);
                break;
            case 2 :
                contacts->MoveIterator(it, contactsList);
                break;
            case 3 :
                contacts->InsertItem(it, contactsList);
                break;
            case 4 :
                contacts->EditItem(it);
                break;
            case 5 :
                contacts->EraseItem(it, contactsList);
                break;
            case 6 :
                PrintContactsList(contactsList);
                break;
            case 7 :
                exit(0);
        }

        contacts->PrintMenu();
    }
    //endregion WorkCycle

    return 0;
}

void PrintContactsList(std::list <Contacts>& contactsList)
{
    for (auto& item : contactsList)
        contacts->PrintItem(item);
}