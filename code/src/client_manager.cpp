//
// Created by acharyab on 3/31/16.
//
#include <iostream>
using namespace std;
#include "../include/client_manager.h"

// constructor of Cat,
Cat::Cat(int initialAge)
{
    itsAge = initialAge;
    string = new char[10]();
}

//copy constructor for making a new copy of a Cat
Cat::Cat(const Cat& copy_from) {
    itsAge = copy_from.itsAge;
    string = new char[10]();
    std::copy(copy_from.string+0, copy_from.string+10, string);
}

//copy assignment for assigning a value from one Cat to another
Cat& Cat::operator=(const Cat& copy_from) {
    itsAge = copy_from.itsAge;
    std::copy(copy_from.string+0, copy_from.string+10, string);
}

Cat::~Cat()                 // destructor, just an example
{
    delete[] string;
}

// GetAge, Public accessor function
// returns value of itsAge member
int Cat::GetAge() const
{
    return itsAge;
}

// Definition of SetAge, public
// accessor function

void Cat::SetAge(int age)
{
    // set member variable its age to
    // value passed in by parameter age
    itsAge = age;
}

// definition of Meow method
// returns: void
// parameters: None
// action: Prints "meow" to screen
void Cat::Meow()
{
    cout << "Meow.\n";
}
