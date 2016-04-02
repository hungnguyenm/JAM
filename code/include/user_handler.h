//
// Created by Krzysztof Jordan on 4/2/16.
//

#include "callable.h"

#ifndef JAM_USER_HANDLER_H
#define JAM_USER_HANDLER_H

class UserHandler : public Callable {
public:
    void operator()();
    boost::thread run_on_thread();
private:

};

#endif //JAM_USER_HANDLER_H
