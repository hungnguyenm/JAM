/**
 * JAM class - Module Coordinator acts as the link between all internal modules.
 * It functions as the single point of contact of JAM.
 *
 * @author: Hung Nguyen, Krzysztof Jordan, Bipeen Acharya
 * @version 1.0 04/07/16
 */

#ifndef JAM_JAM_H
#define JAM_JAM_H

#include "udp_wrapper.h"
#include "user_handler.h"

class JAM {
public:
    JAM();

    ~JAM();

    /**
     * Start JAM as a leader
     *
     * Assume new group, start listening to incoming packets.
     *
     * @param username  user's name
     */
    void StartAsLeader(const char *username);

    /**
     * Start JAM as a client
     *
     * Join group by connecting to server address. If server doesn't respond then JAM returns error.
     *
     * @param username  user's name
     * @param ip        server's ip
     * @param port      server's port
     */
    void StartAsClient(const char *username,
                            const char *ip,
                            const char *port);
};

#endif //JAM_JAM_H
