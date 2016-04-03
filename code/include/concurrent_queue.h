/**
 * Thread-safe blocking queue template
 *
 * @author: Hung Nguyen
 * @version 1.0 04/03/16
 */

#ifndef JAM_CONCURRENT_QUEUE_H
#define JAM_CONCURRENT_QUEUE_H

#include <queue>
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"

template <typename T>
class ConcurrentQueue {
private:
    std::queue<T> queue_;
    mutable boost::mutex m_queue_;
    boost::condition_variable cond_variable_;

public:
    void Push(T const& t) {
        boost::mutex::scoped_lock lock(m_queue_);
        queue_.push(t);
        lock.unlock();
        cond_variable_.notify_one();
    }

};

#endif //JAM_CONCURRENT_QUEUE_H