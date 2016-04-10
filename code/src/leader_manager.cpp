//
// Created by Krzysztof Jordan on 4/9/16.
//

#include "../include/leader_manager.h"

LeaderManager::LeaderManager(ClientManager clientManager) :
        clientManager_(clientManager)
{

}

ClientInfo LeaderManager::GetCurrentLeader() {
    std::vector<ClientInfo> clientInfos = clientManager_.GetAllClients();

    for(int i = 0; i < clientInfos.size(); ++i) {
        if(clientInfos[i].is_leader()) {
            return clientInfos[i];
        }
    }

    // TODO: No leader, needs election?
}