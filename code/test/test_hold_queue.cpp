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

    // In order delivery test
    cout << "--- In order test ---" << endl;

    payload.SetType(CHAT_MSG);
    payload.SetUsername("Dummy1");
    payload.SetMessage("Message 1");
    payload.SetOrder(0);
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy3");
    payload.SetMessage("Message 3");
    payload.SetOrder(2);
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy2");
    payload.SetMessage("Message 2");
    payload.SetOrder(1);
    holdQueue.AddMessageToQueue(payload);

    // Wait
    getchar();

    // Get lost payload test
    cout << "--- Get lost payload test ---" << endl;

    Payload lost_payload;
    bool value = holdQueue.GetPayloadInHistory(2, &lost_payload);

    if (value) {
        cout << lost_payload.GetUsername().c_str() << endl;
    }

    // Wait
    getchar();

    // Duplicate test
    cout << "--- Duplicate test ---" << endl;

    holdQueue.ClearQueue();

    payload.SetType(CHAT_MSG);
    payload.SetUsername("Dummy1");
    payload.SetMessage("Message 1");
    payload.SetOrder(0);
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy2");
    payload.SetMessage("Message 2");
    payload.SetOrder(1);
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy3");
    payload.SetMessage("Message 3");
    payload.SetOrder(2);
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy3");
    payload.SetMessage("Message 3");
    payload.SetOrder(2);
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy4");
    payload.SetMessage("Message 4");
    payload.SetOrder(3);
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy5");
    payload.SetMessage("Message 5");
    payload.SetOrder(4);
    holdQueue.AddMessageToQueue(payload);


    getchar();
    holdQueue.ClearQueue();

    payload.SetType(CHAT_MSG);
    payload.SetUsername("Dummy1");
    payload.SetMessage("Message 1");
    payload.SetOrder(0);
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy2");
    payload.SetMessage("Message 2");
    payload.SetOrder(1);
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy8");
    payload.SetMessage("Message 3");
    payload.SetOrder(9);
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy9");
    payload.SetMessage("Message 3");
    payload.SetOrder(10);
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy10");
    payload.SetMessage("Message 4");
    payload.SetOrder(11);
    holdQueue.AddMessageToQueue(payload);

    payload.SetUsername("Dummy11");
    payload.SetMessage("Message 5");
    payload.SetOrder(12);
    holdQueue.AddMessageToQueue(payload);


    payload.SetUsername("Dummy11");
    payload.SetMessage("Message 5");
    payload.SetOrder(13);
    holdQueue.AddMessageToQueue(payload);


    payload.SetUsername("Dummy11");
    payload.SetMessage("Message 5");
    payload.SetOrder(14);
    holdQueue.AddMessageToQueue(payload);

    // Wait
    getchar();
    return 0;
}