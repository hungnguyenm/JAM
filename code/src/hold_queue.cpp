//
// Created by acharyab on 4/11/16.
//

#include "../include/hold_queue.h"

#include <queue>

HoldQueue::HoldQueue() {

}
HoldQueue::~HoldQueue() {

}

// need to take userhandler inside

void HoldQueue::AddMessage(Payload payload) {
    delivery_queue_.push(payload);
}

void HoldQueue::Process() {

    
    history_queue_.push(payload);
    StreamCommunicator::SendMessage(user_handler_pipe_,
                                    payload.GetUsername(),
                                    payload.GetMessage());


}



