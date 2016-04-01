//
// Created by acharyab on 3/31/16.
//
#include <iostream>
using namespace std;
#include "../include/client_manager.h"

// constructor of ClientManager,
ClientManager::ClientManager()
{
    public:

    string = new char[10]();
}

//
////copy constructor for making a new copy of a Cat
//Cat::Cat(const Cat& copy_from) {
//    itsAge = copy_from.itsAge;
//    string = new char[10]();
//    std::copy(copy_from.string+0, copy_from.string+10, string);
//}
//
////copy assignment for assigning a value from one Cat to another
//Cat& Cat::operator=(const Cat& copy_from) {
//    itsAge = copy_from.itsAge;
//    std::copy(copy_from.string+0, copy_from.string+10, string);
//}

ClientManager::~ClientManager()                 // destructor, just an example
{
    delete[] string;
}