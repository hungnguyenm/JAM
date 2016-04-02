//
// Created by Krzysztof Jordan on 4/2/16.
//

#ifndef JAM_CALLABLE_H
#define JAM_CALLABLE_H

#include <boost/thread.hpp>

class Callable {
public:
    virtual boost::thread run_on_thread() = 0;
    virtual void operator()() = 0;
};

#endif //JAM_CALLABLE_H
