#include <iostream>

#include "../../include/udp_wrapper.h"

using namespace std;

int main() {
    UdpWrapper udpWrapper;

    udpWrapper.Start();

    Payload payload;
    payload.SetType(NA);
    udpWrapper.SendPayload(payload);

    udpWrapper.Stop();

    return 0;
}