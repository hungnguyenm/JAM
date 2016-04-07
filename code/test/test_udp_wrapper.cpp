/**
 * Test program for UDP Wrapper module
 *
 * @author: Hung Nguyen
 * @version 1.0 04/05/16
 */

#include <iostream>

#include <ifaddrs.h>
#include <arpa/inet.h>

#include "../include/udp_wrapper.h"

using namespace std;

int main() {
    UdpWrapper udpWrapper;
    uint32_t port;
    // udpWrapper.Start(&port);

    // Testing ip address
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *iaddr;

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family==AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            iaddr = inet_ntoa(sa->sin_addr);
            printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, iaddr);
        }
    }

    freeifaddrs(ifap);

//    sockaddr_in addr;
//    if (UdpWrapper::GetAddressFromInfo("localhost", "55056", &addr) == SUCCESS) {
//        Payload payload;
//        payload.SetType(CHAT_MSG);
//        payload.SetUsername("Hung");
//        payload.SetMessage("Test Message");
//        payload.EncodePayload();
//
//        udpWrapper.SendPayloadSelf(payload);
//
//        udpWrapper.SendPayloadSingle(payload, &addr);
//    }
//    udpWrapper.Join();      // Run forever

    return 0;
}
