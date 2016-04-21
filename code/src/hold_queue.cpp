//
// Created by acharyab on 4/11/16.
//

#include "../include/hold_queue.h"

#include <algorithm>    // std::sort

HoldQueue::HoldQueue() {

}
HoldQueue::~HoldQueue() {

}

// need to take userhandler inside

void HoldQueue::AddMessageToQueue(Payload payload) {
    delivery_queue_.push_back(payload);
    std::sort(delivery_queue_.begin(), delivery_queue_.end());

    Process(payload);
}

void HoldQueue::Process(Payload payload) {

    history_queue_.push(payload);

    if (payload.GetOrder() == DEFAULT_NO_ORDER) {
        return;
    } else if (payload.GetType() == CHAT_MSG) {

        }
    }
}

Payload HoldQueue::GetPayloadInHistory(int32_t value) {

}



