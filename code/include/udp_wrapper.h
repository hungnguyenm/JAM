/**
 * UDP Wrapper module acts as a low-level I/O handler to send/receive message
 * over UDP datagram reliably.
 *
 * @author: Hung Nguyen
 * @version 1.0 03/31/16
 */

#ifndef JAM_UDP_WRAPPER_H
#define JAM_UDP_WRAPPER_H

#include "config.h"
#include "payload.h"
#include "udp_reader.h"
#include "udp_writer.h"

#include <boost/thread/thread.hpp>

class UdpWrapper {
public:
    UdpWrapper();

    ~UdpWrapper();

    /**
     * Initialize UDP socket and start listening
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus Start();

    // TODO: implement start as client

    /**
     * Terminate all working threads and exit
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus Stop();

    /**
     * Put payload to a single receiver to queue
     * UDP socket must be init before this function can be used.
     *
     * @param addr      address of the receiver (IPv4/port)
     * @param payload   ready to encode payload
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus SendPayload(sockaddr_in addr, Payload payload);

    /**
     * Put payload to a list of receivers to queue
     * UDP socket must be init before this function can be used.
     *
     * @param addr      address of the receivers (IPv4/port)
     * @param payload   ready to encode payload
     *
     * @returns         SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus DistributePayload(std::vector<sockaddr_in> addr, Payload payload);

private:
    bool is_ready_;             // UDP socket ready for communication
    int sockfd_;                // Main socket file descriptor

    UdpReader reader_;
    UdpWriter writer_;

    boost::thread t_reader_;
    boost::thread t_writer_;

    /**
     * Initialize listening UDP socket (bind to specific port)
     *
     * @returns         SUCCESS if bind normally, other JamStatus errors otherwise
     */
    JamStatus InitUdpSocket();
};

#endif //JAM_UDP_WRAPPER_H
