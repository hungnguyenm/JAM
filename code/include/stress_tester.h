//
// Created by Krzysztof Jordan on 4/24/16.
//

#ifndef JAM_STRESS_TESTER_H
#define JAM_STRESS_TESTER_H

#include <string>
#include <boost/thread.hpp>

#include "stream_communicator.h"

class StressTester {
    StressTester(CentralQueues* queues, int millis, const std::string& fileName);

private:
    CentralQueues queues_;
    int delay_;
    std::string fileName_;

    void Run();
};


#endif //JAM_STRESS_TESTER_H
