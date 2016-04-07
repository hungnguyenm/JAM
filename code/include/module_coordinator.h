/**
 * Module Coordinator acts as the link between all internal modules. It functions as the single point
 *  of contact of JAM.
 *
 * @author: Hung Nguyen, Krzysztof Jordan, Bipeen Acharya
 * @version 1.0 04/07/16
 */

#ifndef JAM_MODULE_COORDINATOR_H
#define JAM_MODULE_COORDINATOR_H

#include "udp_wrapper.h"
#include "user_handler.h"

class ModuleCoordinator {
public:
    ModuleCoordinator();

    ~ModuleCoordinator();

    /**
     * Start JAM as a leader
     *
     * Assume new group, start listening to incoming packets.
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus StartAsLeader();

    /**
     * Start JAM as a client
     * 
     * Join group by connecting to server address. If server doesn't respond then JAM returns error.
     *
     * @param ip        server's ip
     * @param port      server's port
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus StartAsClient(const char *ip,
                            const char *port);
};

#endif //JAM_MODULE_COORDINATOR_H
