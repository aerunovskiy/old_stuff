#ifndef OOP_SEM4_CONTACT_H
#define OOP_SEM4_CONTACT_H

#include <iostream>
#include <string>
using namespace std;

class Contact {
private:
    string name;
    string number;
public:
    Contact();
    Contact(string& _name, string& _number);

    string getName();
    void setName(string _name);

    string getNumber();
    void setNumber(string _number);

    Contact& operator=(Contact& contact);
    friend ostream& operator<< (ostream&, Contact& contact);


};


#endif //OOP_SEM4_CONTACT_H
