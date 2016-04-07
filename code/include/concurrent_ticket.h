/**
 * Thread-safe ticket template.
 *
 * Tickets are map with 1 unique key order and multiple values organizes in a tuple.
 *
 * @author: Hung Nguyen
 * @version 1.0 04/05/16
 */

#ifndef JAM_CONCURRENT_TICKET_H
#define JAM_CONCURRENT_TICKET_H

#include <map>
#include <tuple>
#include "boost/thread/mutex.hpp"

template<typename K, typename... V>
class ConcurrentTicket {
private:
    typedef std::map<K, std::tuple<V...>> Ticket;
    Ticket tickets_;
    mutable boost::mutex m_tickets_;

public:
    /**
     * Get map size
     *
     * @return          map size
     */
    size_t size() {
        boost::mutex::scoped_lock lock(m_tickets_);
        return tickets_.size();
    }

    /**
     * Check if map is empty
     *
     * @return          TRUE if map is empty, FALSE otherwise
     */
    bool is_empty() {
        boost::mutex::scoped_lock lock(m_tickets_);
        return tickets_.empty();
    }

    /**
     * Check if element with key in exist in the list
     *
     * @param key       element key
     *
     * @return          TRUE if exists, FALSE otherwise
     */
    bool has_element(K key) {
        boost::mutex::scoped_lock lock(m_tickets_);
        auto search = tickets_.find(key);
        return search != tickets_.end();
    }

    /**
     * Find element with key in the list
     *
     * @param key       element key
     *
     * @return          TRUE if found, FALSE otherwise. Element is sent within pair.
     */
    std::pair<std::pair<K, std::tuple<V...>>, bool> find(K key) {
        boost::mutex::scoped_lock lock(m_tickets_);
        typename Ticket::iterator search = tickets_.find(key);
        return std::make_pair(std::make_pair(search->first, search->second), search != tickets_.end());
    };

    /**
     * Get all elements in the list
     *
     * @return          a vector of tuples of element's key and values
     */
    const std::vector<std::tuple<K, V...>> get_all() {
        boost::mutex::scoped_lock lock(m_tickets_);
        std::vector<std::tuple<K, V...>> ret;
        for (auto const &item : tickets_) {
            ret.push_back(std::tuple_cat(std::make_tuple(item.first), item.second));
        }
        return ret;
    };

    /**
     * Insert element into list if no element with the same key exists
     *
     * @param key       element key
     * @param values    element values
     *
     * @return          TRUE if success, FALSE otherwise
     */
    bool insert(K key, V... values) {
        boost::mutex::scoped_lock lock(m_tickets_);
        auto t_values = std::make_tuple(values...);
        std::pair<typename Ticket::iterator, bool> res =
                tickets_.insert(typename Ticket::value_type(key, t_values));
        return res.second;
    }

    /**
     * Insert element into list if no element with the same key exists
     *  or overwrite the exist element with the same key
     *
     * @param key       element key
     * @param values    element values
     *
     * @return          TRUE if success, FALSE otherwise
     */
    bool insert_or_assign(K key, V... values) {
        boost::mutex::scoped_lock lock(m_tickets_);
        try {
            auto t_values = std::make_tuple(values...);
            tickets_[key] = t_values;
            return true;
        } catch (...) {
            return false;
        }
    }

    /**
     * Remove element with key in the list
     *
     * @param key       element key
     *
     * @return          TRUE if found and remove successfully, FALSE otherwise
     */
    bool erase(K key) {
        boost::mutex::scoped_lock lock(m_tickets_);
        return tickets_.erase(key);
    }
};

#endif //JAM_CONCURRENT_TICKET_H
