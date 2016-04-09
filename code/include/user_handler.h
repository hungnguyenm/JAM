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
#include "central_queues.h"

class UserHandler : public Callable {
public:
    UserHandler(CentralQueues *queues);

    void operator()();

    boost::thread run_on_thread();

    int get_write_pipe();

private:
    CentralQueues *queues_;                         // Central queues for inter-communication

    fd_set activeFdSet_;
    fd_set readFdSet_;

    int incomingFd_[2];

    // Helper functions
    void PrintMessage(const std::string &sender, const std::string &message);
};

#endif //JAM_USER_HANDLER_H
