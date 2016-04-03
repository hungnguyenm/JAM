//
// Created by Krzysztof Jordan on 4/2/16.
//

#ifndef JAM_USER_HANDLER_H
#define JAM_USER_HANDLER_H

#include <string>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

#include "callable.h"

class UserHandler : public Callable {
public:
    UserHandler();
    ~UserHandler();

    void operator()();

    boost::thread run_on_thread();
    int get_read_pipe();
private:
    fd_set activeFdSet_;
    int fd_[2];

    // Helper functions
    void PrintMessage(const std::string& sender, const std::string& message);
};

#endif //JAM_USER_HANDLER_H
