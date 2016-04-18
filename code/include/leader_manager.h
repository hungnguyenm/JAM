//
// Created by Krzysztof Jordan on 4/9/16.
//

#ifndef JAM_LEADER_MANAGER_H
#define JAM_LEADER_MANAGER_H

#include "central_queues.h"
#include "client_manager.h"

class LeaderManager {
public:
    LeaderManager(CentralQueues *queues, ClientManager *clientManager);

    ClientInfo GetCurrentLeader();

private:
    CentralQueues *queues_;                         // Central queues for inter-communication
    ClientManager *clientManager_;

    bool PingLeader();
    void StartElection();
};


#endif //JAM_LEADER_MANAGER_H
