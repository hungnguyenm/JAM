#include <iostream>

#include "../../include/udp_wrapper.h"

using namespace std;

int main() {

    // Test ConcurrentTicket

    UdpWrapper udpWrapper;
    udpWrapper.Start();

    sockaddr_in addr;
    if (UdpWrapper::GetAddressFromInfo("localhost", "55056", &addr) == SUCCESS) {
        Payload payload;
        payload.SetType(CHAT_MSG);
        payload.SetUsername("Hung");
        payload.SetMessage("Test Message");
        payload.EncodePayload();

        udpWrapper.SendPayloadSelf(payload);
    }
    udpWrapper.Join();  // Run forever

    return 0;
}