/**
 * Test program for UDP Wrapper module
 *
 * @author: Hung Nguyen
 * @version 1.0 04/05/16
 */

#include <iostream>

#include "../include/udp_wrapper.h"

using namespace std;

int main() {
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
    udpWrapper.Join();

    return 0;
}
