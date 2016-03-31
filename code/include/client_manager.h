//
// Created by acharyab on 3/31/16.
//

#ifndef JAM_CLIENTMANAGER_H
#define JAM_CLIENTMANAGER_H


class Cat {
    public:                    // begin public section
        Cat(int initialAge);     // constructor
        Cat(const Cat& copy_from); //copy constructor
        Cat& operator=(const Cat& copy_from); //copy assignment
        ~Cat();                  // destructor

        int GetAge() const;            // accessor function
        void SetAge(int age);    // accessor function
        void Meow();
    private:                   // begin private section
        int itsAge;              // member variable
        char * string;
};


#endif //JAM_CLIENTMANAGER_H
