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
     * Update internal client list
     *
     * @param clients   list of client sockaddr_in
     */
    void UpdateClientList(std::vector<sockaddr_in> *clients);

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
     * Put payload to client list to queue
     * UDP socket must be init before this function can be used.
     *
     * @param payload   ready to encode payload
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus DistributePayload(Payload payload);

private:
    bool is_ready_;                     // UDP socket ready for communication
    int sockfd_;                        // Main socket file descriptor
    std::vector<sockaddr_in> clients_;  // Up-to-date client list
    uint32_t uid_;                      // UID counter

    boost::thread t_reader_;            // Reader thread for RunReader()
    boost::thread t_writer_;            // Writer thread for RunWriter()

    /**
     * Initialize listening UDP socket (bind to specific port)
     *
     * @returns         SUCCESS if bind normally, other JamStatus errors otherwise
     */
    JamStatus InitUdpSocket();

    /**
     * Start reader thread to listen for incoming packets.
     */
    void RunReader();

    /**
     * Start writer thread to distribute packets.
     */
    void RunWriter();
};

#endif //JAM_UDP_WRAPPER_H
