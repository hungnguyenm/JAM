/**
 * Central Queues is used for inter-communication between modules in JAM.
 * It has multiple thread-safe queues with a single condition variable to wake up JAM.
 *
 * @author: Hung Nguyen
 * @version 1.0 04/07/16
 */

#ifndef JAM_CENTRAL_QUEUES_H
#define JAM_CENTRAL_QUEUES_H

#ifdef SECURE
#include "payload_secure.h"
#else
#include "payload.h"
#endif

#include "concurrent_queue.h"

#include <sys/socket.h>
#include "boost/variant.hpp"

class CentralQueues {
public:
    typedef boost::variant<Payload, sockaddr_in, int32_t> QueueParam;

    enum QueueType {
        USER_OUT,
        UDP_IN,
        UDP_CRASH,
        LEADER_OUT,
        HISTORY_REQUEST
    };

    CentralQueues();

    ~CentralQueues();

    /**
     * Get queue size
     *
     * @param type      queue selected
     *
     * @return          queue size
     */
    size_t size(QueueType type);

    /**
     * Check if queue is empty
     *
     * @param type      queue selected
     *
     * @return          TRUE if queue is empty, FALSE otherwise
     */
    bool is_empty(QueueType type);

    /**
     * Insert element into selected queue
     *
     * @param type      queue selected
     * @param in        element to be inserted
     */
    void push(QueueType type, QueueParam const &in);

    /**
     * Check if queue is empty
     *
     * @param time      time to wait if no data in miliseconds
     *
     * @return          TRUE if queue is there is data, FALSE otherwise
     */
    bool wait_for_data(uint32_t time);

    /**
     * Get element from queue if not empty
     * (need to have separate function for each queue as compiler requirements)
     *
     * @param out       element from queue if available
     *
     * @return          TRUE if has data returned, FALSE otherwise
     */
    bool try_pop_user_out(Payload &out);

    bool try_pop_udp_in(Payload &out);

    bool try_pop_udp_crash(sockaddr_in &out);

    bool try_pop_leader_out(Payload &out);

    bool try_pop_history_request(int32_t &out);

    /**
     * Set terminate flag to TRUE
     */
    void signal_terminate();

    bool is_terminate();

private:
    ConcurrentQueue<Payload> user_out_queue_;           // Thread-safe outgoing message payload queue from UserHandler

    ConcurrentQueue<Payload> udp_in_queue_;             // Thread-safe incoming payload queue for processing
    ConcurrentQueue<sockaddr_in> udp_crash_queue;       // Thread-safe crash notification queue from UdpHandler

    ConcurrentQueue<Payload> leader_out_queue_;         // Thread-safe outgoing payload queue for processing

    ConcurrentQueue<int32_t> history_request_queue_;    // Thread-safe queue for HoldQueue to request missing payload

    mutable boost::mutex m_cond_;                       // mutex just for condition variable
    mutable boost::mutex m_exit_;                       // mutex for exit_
    boost::condition_variable cond_variable_;           // To notify change happens in one of the queues

    bool exit_;                                         // Signal to terminate the chat
};

#endif //JAM_CENTRAL_QUEUES_H
