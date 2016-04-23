//
// Created by Hung Nguyen on 4/23/16.
//

#include "../include/jam.h"

using namespace std;

int main() {
    // These are for initializing support classes
    CentralQueues queues;
    UserHandler userHandler(&queues);
    HoldQueue holdQueue(&queues);

    userHandler.Start();
    holdQueue.SetUserHandlerPipe(userHandler.get_write_pipe());

    // Construct payload
    Payload payload;
    payload.SetType(CHAT_MSG);
    payload.SetUsername("Dummy");
    payload.SetMessage("Message 1");
    payload.SetOrder(1);

    // Pump to queue
    holdQueue.AddMessageToQueue(payload);

    // .... other testing here .....

    return 0;
}