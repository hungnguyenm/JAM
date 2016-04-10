//
// Created by Krzysztof Jordan on 4/9/16.
//

#ifndef JAM_LEADER_MANAGER_H
#define JAM_LEADER_MANAGER_H

#include "client_manager.h"

class LeaderManager {
public:
    LeaderManager(ClientManager clientManager);

    ClientInfo GetCurrentLeader();

private:
    ClientManager clientManager_;
};


#endif //JAM_LEADER_MANAGER_H
