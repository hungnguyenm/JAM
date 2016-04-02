//
// Created by Krzysztof Jordan on 4/2/16.
//

#include "../include/user_handler.h"
#include <iostream>

boost::thread UserHandler::run_on_thread() {
    return boost::thread(*this);
}

void UserHandler::operator()() {
    std::cout << "test" << std::endl;
}