/* ---------------------------------------------------------------------------
 * Just Another Messenger (JAM)
 *
 * udp_wrapper.h
 * UDP Wrapper module acts as a low-level I/O handler to send/receive message
 * over UDP datagram reliably.
 *
 * @author: Hung Nguyen
 * @version 1.0 03/31/16
 * -------------------------------------------------------------------------*/

#ifndef JAM_UDP_WRAPPER_H
#define JAM_UDP_WRAPPER_H

#include "payload.h"

class UdpWrapper {
public:
    UdpWrapper();

    ~UdpWrapper();

    /*
     * Function: SendPayload
     * --------------------
     * Put payload to a single receiver to queue
     *
     *  addr: address of the receiver (IPv4/port)
     *  payload: ready to encode payload
     *
     *  returns: SUCCESS on normal operation
     *           other JamStatus errors otherwise
     */
    JamStatus SendPayload(sockaddr_in addr, Payload payload);

    /*
     * Function: DistributePayload
     * --------------------
     * Put payload to a list of receivers to queue
     *
     *  addr: address of the receivers (IPv4/port)
     *  payload: ready to encode payload
     *
     *  returns: SUCCESS on normal operation
     *           other JamStatus errors otherwise
     */
    JamStatus DistributePayload(std::vector<sockaddr_in> addr, Payload payload);

private:
};

#endif //JAM_UDP_WRAPPER_H
