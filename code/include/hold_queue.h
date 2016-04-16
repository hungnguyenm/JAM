//
// Created by acharyab on 4/11/16.
//

#ifndef JAM_HOLD_QUEUE_H
#define JAM_HOLD_QUEUE_H

#include <queue>
#include "payload.h"

class HoldQueue {
public:                    // begin public section
    HoldQueue();
    ~HoldQueue();



private:
    std::queue<Payload> hold_back_queue_;
    std::queue<Payload> delivery_queue_;

}

#endif //JAM_HOLD_QUEUE_H
