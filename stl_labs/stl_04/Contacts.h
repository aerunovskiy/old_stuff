#ifndef OOP_SEM4_CONTACTS_H
#define OOP_SEM4_CONTACTS_H

//region Include
#include <iostream>
#include <string>
#include <list>
//endregion Include

class Contacts {
private:
    typedef std::list<Contacts>::iterator t_contactsit;
public:
    std::string name;
    std::string number;

    Contacts() { name = "Default"; number = "+7 (999) 999 99 99"; };
    Contacts (std::string _name, std::string _number);

    //Contacts (string _name, string _number);

    void FirstInit(std::list <Contacts>& contactsList);

    void PrintItem(Contacts& contact);

    void PrintMenu();

    void MoveIterator(t_contactsit& it, std::list<Contacts>& contactsList);

    void InsertItem(t_contactsit& it, std::list<Contacts>& contactsList);

    void EraseItem(t_contactsit& it, std::list<Contacts>& contactsList);

    void EditItem(t_contactsit& it);
};


#endif //OOP_SEM4_CONTACTS_H
