/**
 * UDP Wrapper module acts as a low-level I/O handler to send/receive message
 * over UDP datagram reliably.
 *
 * @author: Hung Nguyen
 * @version 1.0 03/31/16
 */

#ifndef JAM_UDP_WRAPPER_H
#define JAM_UDP_WRAPPER_H

#include "payload.h"

class UdpWrapper {
public:
    UdpWrapper();

    ~UdpWrapper();

    /**
     *
     */
    Start();

    /**
     * Put payload to a single receiver to queue
     *
     * @param addr      address of the receiver (IPv4/port)
     * @param payload   ready to encode payload
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus SendPayload(sockaddr_in addr, Payload payload);

    /**
     * Put payload to a list of receivers to queue
     *
     * @param addr      address of the receivers (IPv4/port)
     * @param payload   ready to encode payload
     *
     * @returns         SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus DistributePayload(std::vector<sockaddr_in> addr, Payload payload);

private:
};

#endif //JAM_UDP_WRAPPER_H
