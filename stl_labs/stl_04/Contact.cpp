#include "Contact.h"

Contact::Contact()
: name  ("Default"),
  number("+7 (999) 999 99 99")
{}

Contact::Contact(string& _name, string& _number)
: name  (_name),
  number(_number)
{}

string Contact::getName  () { return name;   }
string Contact::getNumber() { return number; }