//
// Created by acharyab on 4/11/16.
//

#include "../include/hold_queue.h"

HoldQueue::HoldQueue(CentralQueues *queues) :
        queues_(queues), user_handler_pipe_(-1),
        expected_order_(DEFAULT_FIRST_ORDER), recovery_counter_(0) {
    delivery_queue_.reserve(MAX_HOLDBACK_QUEUE_LENGTH);
}

HoldQueue::~HoldQueue() {
}

void HoldQueue::AddMessageToQueue(Payload payload) {

    if (payload.GetType() == CHAT_MSG) {

        //prevent duplicate sending so goes through history queue
        // and returns without doing anything
        // if the payload has already been sent (duplicate)
        std::deque<Payload>::iterator it = history_queue_.begin();
        while (it != history_queue_.end()) {
            if (payload.GetOrder() == it->GetOrder()) {
                DCOUT("WARNING: HoldQueue - Trying to send duplicate message-ignored");
                return;
            }
            ++it;
        }

        delivery_queue_.push_back(payload);
        std::sort(delivery_queue_.begin(), delivery_queue_.end());
        recovery_counter_ ++;
        ProcessPayloads();
    }


}

void HoldQueue::ProcessPayloads() {

    while(delivery_queue_.size() > 0 && delivery_queue_[0].GetOrder() == expected_order_) {
        Payload payload = delivery_queue_[0];

        StreamCommunicator::SendMessage(user_handler_pipe_,
                                        payload.GetUsername(),
                                        payload.GetMessage());
        recovery_counter_ = 0;
        history_queue_.push_back(payload); //add sent payload to history queue
        delivery_queue_.erase(delivery_queue_.begin()); //remove sent message from delivery queue
        expected_order_ += 1; //increase the expected order of the next message
    }

    if (recovery_counter_ >= NUM_MISSING_ORDER) {
        DCOUT("WARNING: HoldQueue - Requesting history message order = " + expected_order_);
        queues_->push(CentralQueues::HISTORY_REQUEST, expected_order_);
        recovery_counter_ = 0;
    }
}

bool HoldQueue::GetPayloadInHistory(int32_t order, Payload* payload) {
    std::deque<Payload>::iterator it = history_queue_.begin();

    while (it != history_queue_.end()) {
        if (order == it->GetOrder()) {
            payload = &*it;
            return true;
        }
        ++it;
    }
    return false;
}

void HoldQueue::SetUserHandlerPipe(int pipeId) {
    user_handler_pipe_ = pipeId;
}

void HoldQueue::ClearQueue() {
    delivery_queue_.clear();
    history_queue_.clear();
    expected_order_ = DEFAULT_FIRST_ORDER;
}



