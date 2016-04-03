/* ---------------------------------------------------------------------------
 * Just Another Messenger (JAM)
 *
 * udp_wrapper.cpp
 * UDP Wrapper module acts as a low-level I/O handler to send/receive message
 * over UDP datagram reliably.
 *
 * @author: Hung Nguyen
 * @version 1.0 03/31/16
 * -------------------------------------------------------------------------*/

#include "../include/udp_wrapper.h"

using namespace std;

UdpWrapper::UdpWrapper() {

}

UdpWrapper::~UdpWrapper() {

}

JamStatus UdpWrapper::SendPayload(sockaddr_in addr, Payload payload) {
    JamStatus ret = SUCCESS;

    return ret;
}

JamStatus UdpWrapper::DistributePayload(std::vector<sockaddr_in> addr, Payload payload) {
    JamStatus ret = SUCCESS;

    return ret;
}