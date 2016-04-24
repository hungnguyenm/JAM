//
// Created by Krzysztof Jordan on 4/24/16.
//

#include "../include/stress_tester.h"

#include <fstream>

StressTester::StressTester(CentralQueues* queues, int millis, const std::string& fileName) :
    fd_(fd), delay_(millis), fileName_(fileName) {
        boost::thread(boost::bind(StressTester::Run, this));
}

void StressTester::Run() {
    boost::this_thread::sleep(boost::posix_time::milliseconds(delay_));
    std::ifstream file;
    file.open (fileName_);

    std::string line;
    Payload payload;
    payload.SetType(MessageType::CHAT_MSG);
    DCOUT("INFO: ST -  Beginning stress test");
    while (std::getline(infile, line))
    {
        payload.SetMessage(line);
        queues_->push(CentralQueues::QueueType::USER_OUT, payload);
    }
}