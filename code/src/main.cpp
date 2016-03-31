#include <iostream>

using namespace std;
#include "../include/client_manager.h"

// create a cat, set its age, have it
// meow, tell us its age, then meow again.
int main()
{
    int Age;
    cout<<"How old is Frisky? ";
    cin>>Age;
    Cat Frisky(Age);
    Frisky.Meow();
    cout << "Frisky is a cat who is " ;
    cout << Frisky.GetAge() << " years old.\n";
    Frisky.Meow();
    Age++;
    Frisky.SetAge(Age);
    cout << "Now Frisky is " ;
    cout << Frisky.GetAge() << " years old.\n";
    return 0;
}