//
// Created by acharyab on 4/11/16.
//

#ifndef JAM_HOLD_QUEUE_H
#define JAM_HOLD_QUEUE_H

#ifdef SECURE
#include "payload_secure.h"
#else
#include "payload.h"
#endif

#include "stream_communicator.h"
#include "central_queues.h"

#include <queue>

class HoldQueue {

public:                    // begin public section
    HoldQueue(CentralQueues* queues);
    ~HoldQueue();

    void AddMessageToQueue(Payload payload);
    void ProcessPayloads();

    bool GetPayloadInHistory(int32_t value, Payload* payload);

    void SetUserHandlerPipe(int pipeId);

    void ClearQueue();

private:
    CentralQueues* queues_;
    std::deque<Payload> history_queue_;
    std::vector<Payload> delivery_queue_;

    int user_handler_pipe_;
    int expected_order_;
    int recovery_counter_;

    bool first_payload_;
};

#endif //JAM_HOLD_QUEUE_H
