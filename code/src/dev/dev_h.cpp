#include <iostream>

#include "../../include/udp_wrapper.h"

using namespace std;

int main() {
    UdpWrapper udpWrapper;

    udpWrapper.Start();

    udpWrapper.Stop();

    return 0;
}