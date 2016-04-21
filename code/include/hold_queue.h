//
// Created by acharyab on 4/11/16.
//

#ifndef JAM_HOLD_QUEUE_H
#define JAM_HOLD_QUEUE_H

#include <queue>
#include "payload.h"
#include "stream_communicator.h"

class HoldQueue {

public:                    // begin public section
    HoldQueue();
    ~HoldQueue();

    void AddMessage(Payload payload);
    void Process();

    void HistoryRequest();

private:
    std::queue<Payload> history_queue_;
    std::queue<Payload> delivery_queue_;


    int user_handler_pipe_;

};

#endif //JAM_HOLD_QUEUE_H
