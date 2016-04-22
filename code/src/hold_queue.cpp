//
// Created by acharyab on 4/11/16.
//

#include "../include/hold_queue.h"

HoldQueue::HoldQueue(CentralQueues *queues) :
        queues_(queues), user_handler_pipe_(-1), expected_order_(DEFAULT_FIRST_ORDER) {
}

HoldQueue::~HoldQueue() {

}

void HoldQueue::AddMessageToQueue(Payload payload) {
    if (payload.GetType() == CHAT_MSG) {
        delivery_queue_.push_back(payload);
    }

    std::sort(delivery_queue_.begin(), delivery_queue_.end());

    Process(payload);
}

void HoldQueue::Process(Payload payload) {

    history_queue_.push(payload);

    if (payload.GetOrder() == DEFAULT_NO_ORDER) {
        return;
    }

    do {
        if (payload.GetOrder() == expected_order_) {
            StreamCommunicator::SendMessage(user_handler_pipe_,
                                            payload.GetUsername(),
                                            payload.GetMessage());

            delivery_queue_.erase(delivery_queue_.begin());
            expected_order_ += 1;
        }

    } while (delivery_queue_.size() > 0);
}

bool HoldQueue::GetPayloadInHistory(int32_t value, Payload* payload) {

}

void HoldQueue::SetUserHandlerPipe(int pipeId) {
    user_handler_pipe_ = pipeId;
}

int HoldQueue::GetUserHandlerPipe(){
    return user_handler_pipe_;
}



