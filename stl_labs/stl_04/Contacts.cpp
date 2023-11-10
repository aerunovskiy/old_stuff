#include "Contacts.h"

void Contacts::PrintItem(Contacts& contact)
{
    std::cout << contact.name << " " << contact.number << std::endl;
}

Contacts::Contacts (std::string _name, std::string _number)
{
    name   = _name;
    number = _number;
}

void Contacts::FirstInit(std::list <Contacts>& contactsList)
{
    Contacts contact1 ("Alex", "89021378111");
    Contacts contact2 ( "Denis", "89214378222");
    Contacts contact3 ( "Nick",  "89021378333");

    contactsList = {contact1, contact2, contact3};
}

void Contacts::PrintMenu()
{
    std::cout << "\nMenu\n\n";

    std::cout << "1. Current\n";
    std::cout << "2. Move\n";
    std::cout << "3. Insert item...\n";
    std::cout << "4. Edit item\n";
    std::cout << "5. Erase\n";
    std::cout << "6. Print all items\n";
    std::cout << "7. Exit\n";

    std::cout << "Enter: ";
}

void Contacts::MoveIterator(t_contactsit& it, std::list<Contacts>& contactsList)
{
    int n = 1;
    bool goodN;
    auto first = contactsList.begin();
    auto last  = --contactsList.end();

    while (n != 0)
    {
        goodN = false;
        std::cout << "Enter n to move forward, or -n to move backward. 0 is exit:  ";
        std::cin >> n;

        if ((n > 0 && n <= distance(it, last)) || ((n < 0) && abs(n) <= distance(it, first)))
        {
            goodN = true;
        } else
        {
            std::cout << "Wrong n!\n";
        }

        if (goodN)
        {
            advance(it, n);
            std::cout << "Current item:\n";
            PrintItem(*it);
        }
    }
}

bool IsRightNumber(std::string& number)
{
    if (number.size() != 10)
    {
        std::cout << "Number should has 10 digits, for example \"1234567890\"\n";
        return false;
    }

    size_t found = number.find_first_not_of("1234567890");

    if (found != std::string::npos)
    {
        std::cout << "Only digits, please.\n";
        return false;
    }


    return true;

}

std::string EditNumber(std::string& number)
{
    return "+7 (" + number.substr(0,3) + ") " + number.substr(3,3)
        + " " + number.substr(6,2) + " " + number.substr(8,2);

}

void Contacts::InsertItem(t_contactsit& it, std::list<Contacts>& contactsList)
{
    short key;
    std::string rightNumber;

    while (key != 4)
    {
        Contacts newContact;
        std::cout << "1. Insert Item before current\n";
        std::cout << "2. Insert Item after current\n";
        std::cout << "3. Insert Item to the end\n";
        std::cout << "4. Exit\n";

        std::cin >> key;

        if (key != 4)
        {
            std::cout << "Enter name: ";
            std::cin >> newContact.name;

            std::cout << "Enter number: +7 ";
            std::cin >> rightNumber;

            while (!IsRightNumber(rightNumber))
            {
                std::cout << "Enter number: +7 ";
                std::cin >> rightNumber;
            }

            newContact.number = EditNumber(rightNumber);
        }



        switch (key)
        {
            case 1:
                contactsList.insert(it, newContact);
                break;
            case 2:
                contactsList.insert(++it, newContact);
                break;
            case 3:
                contactsList.push_back(newContact);
                break;
            default:
                std::cout << "Goodbye\n";
                exit;
        }

        std::cout << "New contact: ";
        PrintItem(newContact);
    }
}

void Contacts::EraseItem(t_contactsit& it, std::list<Contacts>& contactsList)
{
    if (contactsList.empty())
    {
        std::cout << "Nothing to erase";
    }
    else
    {
        std::string _name = it->name;
        contactsList.erase(it++);
        std::cout << "Contact \"" << _name << "\" is delete!\n";
    }
}

void Contacts::EditItem(t_contactsit& it)
{
    //Contacts newContact;
    std::cout << "Current item: \n";
    PrintItem(*it);

    std::cout << "New name: ";
    std::cin >> it->name;

    std::cout << "New number: ";
    std::cin >> it->name;

    std::cout << "Current item after edit: \n";
    PrintItem(*it);
}