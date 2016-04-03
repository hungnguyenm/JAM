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

template<typename T>
class ConcurrentQueue {
private:
    std::queue<T> queue_;
    mutable boost::mutex m_queue_;
    boost::condition_variable cond_variable_;

public:
    /**
     * Get queue size
     *
     * @return          queue size
     */
    size_t size() {
        boost::mutex::scoped_lock lock(m_queue_);
        return queue_.size();
    }

    /**
     * Check if queue is empty
     *
     * @return          TRUE if queue is empty, FALSE otherwise
     */
    bool is_empty() {
        boost::mutex::scoped_lock lock(m_queue_);
        return queue_.empty();
    }

    /**
     * Insert element into queue
     *
     * @param t         element to be inserted
     */
    void Push(T const &t) {
        boost::mutex::scoped_lock lock(m_queue_);
        queue_.push(t);
        lock.unlock();
        cond_variable_.notify_all();
    }

    /**
     * Get element from queue
     *
     * @param t         element to be assigned value
     */
    void Pop(T &t) {
        boost::mutex::scoped_lock lock(m_queue_);
        while (queue_.empty()) {
            cond_variable_.wait(lock);
        }

        t = queue_.front();
        queue_.pop();
    }
};

#endif //JAM_CONCURRENT_QUEUE_H