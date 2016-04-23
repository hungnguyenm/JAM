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
    payload.SetUsername("Dummy1");
    payload.SetMessage("Message 1");
    payload.SetOrder(1);

    // Pump to queue
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy2");
    payload.SetMessage("Message 2");
    payload.SetOrder(2);

    // Pump to queue
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy3");
    payload.SetMessage("Message 3");
    payload.SetOrder(3);

    holdQueue.AddMessageToQueue(payload);

    // .... other testing here .....

    holdQueue.ClearQueue();

    Payload *lost_payload;

    bool value = holdQueue.GetPayloadInHistory(3, lost_payload);

    if (value) {
        cout << lost_payload->GetUsername().c_str() << endl;
    }

    payload.SetType(CHAT_MSG);
    payload.SetUsername("Dummy1");
    payload.SetMessage("Message 1");
    payload.SetOrder(1);

    // Pump to queue
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy2");
    payload.SetMessage("Message 2");
    payload.SetOrder(2);

    // Pump to queue
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy3");
    payload.SetMessage("Message 3");
    payload.SetOrder(3);

    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy3");
    payload.SetMessage("Message 3");
    payload.SetOrder(3);

    // Pump to queue
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy4");
    payload.SetMessage("Message 4");
    payload.SetOrder(4);

    // Pump to queue
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy5");
    payload.SetMessage("Message 5");
    payload.SetOrder(5);

    holdQueue.AddMessageToQueue(payload);



    return 0;
}