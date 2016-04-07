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
#include "concurrent_queue.h"
#include "concurrent_ticket.h"

#include <boost/thread/thread.hpp>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <chrono>

class UdpWrapper {
public:
    UdpWrapper();

    ~UdpWrapper();

    /**
     * Initialize UDP socket and start listening
     *
     * @param port      port to bind
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus Start(const char *port);

    // TODO: implement start as client

    /**
     * Terminate all working threads and exit
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus Stop();

    void Join();

    /**
     * Update internal client addresses
     *
     * @param clients   list of client sockaddr_in
     */
    void UpdateClientAddresses(std::vector<sockaddr_in> *clients);

    /**
     * Put payload to a single receiver to queue
     *
     * UDP socket must be init before this function can be used.
     * Receiver's address must be encoded in the payload.
     *
     * @param payload   encoded payload
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus SendPayloadSingle(Payload payload, const sockaddr_in *addr);

    /**
     * Put payload to self to queue
     *
     * UDP socket must be init before this function can be used.
     * Address will be encoded by UdpWrapper.
     *
     * @param payload   encoded payload (to ensure caller validate encoding)
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus SendPayloadSelf(Payload payload);

    /**
     * Put payload to list of clients to queue
     *
     * UDP socket must be init before this function can be used.
     * Address will be encoded by UdpWrapper.
     *
     * @param payload   encoded payload (to ensure caller validate encoding)
     * @param list      list of receivers' addresses
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus SendPayloadList(Payload payload, std::vector<sockaddr_in> *list);

    /**
     * Put payload to all known clients to queue
     *
     * UDP socket must be init before this function can be used.
     * Wrapper will use internal client list for distributing.
     *
     * @param payload   encoded payload (to ensure caller validate encoding)
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus DistributePayload(Payload payload);

    /**
     * Convert IP address and port to sockaddr_in type
     *
     * @param ip        ip address or host name
     * @param port      port
     * @param addr      return correspond sockaddr_in
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    static JamStatus GetAddressFromInfo(const char *addr, const char *port, sockaddr_in *sockaddr);

private:
    enum {
        NUM_UDP_TERMINATE_RETRIES = 10              // Terminate flag for monitor thread
    };

    bool is_ready_;                                 // UDP socket ready for communication
    int sockfd_;                                    // Main socket file descriptor
    sockaddr_in this_addr_;                         // This client's address
    std::vector<sockaddr_in> clients_;              // Up-to-date client addresses
    uint32_t uid_;                                  // UID counter

    ConcurrentQueue<Payload> out_queue_;            // Thread-safe outgoing payload queue for distributing
    ConcurrentQueue<Payload> in_queue_;             // Thread-safe incoming payload queue for processing
    // TODO: optimize monitor algorithm to reduce payload overhead
    ConcurrentTicket<uint32_t, uint8_t,
            std::chrono::milliseconds,
            Payload> ack_tickets_;                  // Thread-safe outgoing payload ticket monitoring
    ConcurrentQueue<sockaddr_in> crash_queue;       // Thread-safe crash notification queue

    boost::thread t_reader_;                        // Reader thread for RunReader()
    boost::thread t_writer_;                        // Writer thread for RunWriter()
    boost::thread t_monitor_;                       // Monitor thread for RunMonitor()

    /**
     * Initialize listening UDP socket (bind to specific port)
     *
     * @param port      port to bind
     *
     * @returns         SUCCESS if bind normally, other JamStatus errors otherwise
     */
    JamStatus InitUdpSocket(const char *port);

    /**
     * Start reader thread to listen for incoming packets.
     */
    void RunReader();

    /**
     * Start writer thread to distribute packets.
     */
    void RunWriter();

    /**
     * Start monitor thread to keep track of non-ack packets.
     */
    void RunMonitor();

    // -- Helper functions
    std::string u32_to_string(uint32_t in);

    bool already_received(std::deque<std::tuple<in_addr_t, in_port_t, uint32_t>> *queue,
                          in_addr_t ip,
                          in_port_t port,
                          uint32_t uid);
};

#endif //JAM_UDP_WRAPPER_H
