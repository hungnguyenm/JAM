/**
 * Central Queues is used for inter-communication between modules in JAM.
 * It has multiple thread-safe queues with a single condition variable to wake up JAM.
 *
 * @author: Hung Nguyen
 * @version 1.0 04/07/16
 */

#include "../include/central_queues.h"

CentralQueues::CentralQueues() :
        exit_(false) {

}

CentralQueues::~CentralQueues() {

}

size_t CentralQueues::size(QueueType type) {
    switch (type) {
        case USER_OUT:
            return user_out_queue_.size();
        case UDP_IN:
            return udp_in_queue_.size();
        case UDP_CRASH:
            return udp_crash_queue.size();
        case LEADER_OUT:
            return leader_out_queue_.size();
        case HISTORY_REQUEST:
            return history_request_queue_.size();
    }
}

bool CentralQueues::is_empty(QueueType type) {
    switch (type) {
        case USER_OUT:
            return user_out_queue_.is_empty();
        case UDP_IN:
            return udp_in_queue_.is_empty();
        case UDP_CRASH:
            return udp_crash_queue.is_empty();
        case LEADER_OUT:
            return leader_out_queue_.is_empty();
        case HISTORY_REQUEST:
            return history_request_queue_.is_empty();
    }
}

void CentralQueues::push(QueueType type, QueueParam const &in) {
    switch (type) {
        case USER_OUT:
            user_out_queue_.push(boost::get<Payload>(in));
            break;
        case UDP_IN:
            udp_in_queue_.push(boost::get<Payload>(in));
            break;
        case UDP_CRASH:
            udp_crash_queue.push(boost::get<sockaddr_in>(in));
            break;
        case LEADER_OUT:
            leader_out_queue_.push(boost::get<Payload>(in));
            break;
        case HISTORY_REQUEST:
            history_request_queue_.push(boost::get<int32_t>(in));
            break;
    }
    cond_variable_.notify_all();
}

bool CentralQueues::wait_for_data(uint32_t time) {
    boost::mutex::scoped_lock lock(m_cond_);
    while (user_out_queue_.is_empty() &&
           udp_in_queue_.is_empty() &&
           udp_crash_queue.is_empty() &&
           leader_out_queue_.is_empty() &&
           history_request_queue_.is_empty()) {
        cond_variable_.timed_wait(lock, boost::posix_time::milliseconds(time));
    }

    return (!user_out_queue_.is_empty() ||
            !udp_in_queue_.is_empty() ||
            !udp_crash_queue.is_empty() ||
            !leader_out_queue_.is_empty() ||
            !history_request_queue_.is_empty() ||
            exit_);
}

bool CentralQueues::try_pop_user_out(Payload &out) {
    return user_out_queue_.try_pop(out);
}

bool CentralQueues::try_pop_udp_in(Payload &out) {
    return udp_in_queue_.try_pop(out);
}

bool CentralQueues::try_pop_udp_crash(sockaddr_in &out) {
    return udp_crash_queue.try_pop(out);
}

bool CentralQueues::try_pop_leader_out(Payload &out) {
    return leader_out_queue_.try_pop(out);
}

bool CentralQueues::try_pop_history_request(int32_t &out) {
    return history_request_queue_.try_pop(out);
}

void CentralQueues::signal_terminate() {
    boost::mutex::scoped_lock lock(m_exit_);
    exit_ = true;
    lock.unlock();
    cond_variable_.notify_all();
}

bool CentralQueues::is_terminate() {
    boost::mutex::scoped_lock lock(m_exit_);
    return exit_;
}