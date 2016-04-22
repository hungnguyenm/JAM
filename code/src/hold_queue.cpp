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
    // TODO: change this - for JAM testing
    StreamCommunicator::SendMessage(user_handler_pipe_,
                                    payload.GetUsername(),
                                    payload.GetMessage());

    return;

    if (payload.GetType() == CHAT_MSG) {
        delivery_queue_.push_back(payload);
    }

    std::sort(delivery_queue_.begin(), delivery_queue_.end());

    Process(payload);
}

void HoldQueue::Process(Payload payload) {

    if (payload.GetOrder() == DEFAULT_NO_ORDER) {
        return;
    }

    do {
        if (payload.GetOrder() == expected_order_) { //if payload is of the right order
            StreamCommunicator::SendMessage(user_handler_pipe_,
                                            payload.GetUsername(),
                                            payload.GetMessage());
            history_queue_.push(payload); //add sent payload to history queue
            delivery_queue_.erase(delivery_queue_.begin()); //remove sent message from delivery queue
            expected_order_ += 1; //increase the expected order of the next message
            payload = delivery_queue_.front(); //update payload to be the front of the message
        } else if (payload.GetOrder() != expected_order_) { //if payload is not of right order
            //TODO: ask for missing payload from somewhere

        }
    } while (delivery_queue_.size() > 0);
}

bool HoldQueue::GetPayloadInHistory(int32_t value, Payload* payload) {

    if (value < history_queue_.size()) {
        payload = history_queue_.
    }
    return false;
}

void HoldQueue::SetUserHandlerPipe(int pipeId) {
    user_handler_pipe_ = pipeId;
}

int HoldQueue::GetUserHandlerPipe(){
    return user_handler_pipe_;
}



