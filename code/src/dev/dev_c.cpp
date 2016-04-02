//
// Created by Hung Nguyen on 3/31/16.
// Edited by Krzysztof Jordan from then on :P
//

#include "../../include/user_handler.h"

int main(int argc, char* argv[])    {
    UserHandler test;
    boost::thread t = test.run_on_thread();

    t.join();
    return 0;
}