//
// Created by Krzysztof Jordan on 4/9/16.
//

#include "../include/leader_manager.h"
#include "../include/payload.h"

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

    StartElection();
}

bool LeaderManager::PingLeader() {
    Payload payload;
}

void LeaderManager::StartElection() {
    std::vector<ClientInfo> higherOrderClients  = clientManager_.GetHigherOrderClients();

    if(higherOrderClients.size() == 0) {
       // Declare yourself to be the winner and tell anyone who sent you this message to stop
        return;
    }

    for(int i = 0; i < higherOrderClients.size(); i++) {
        // tell each higher order client that you are declaring yourself to be a leader
    }
}