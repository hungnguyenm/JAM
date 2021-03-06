//
// Created by Krzysztof Jordan on 4/9/16.
//

#ifndef JAM_LEADER_MANAGER_H
#define JAM_LEADER_MANAGER_H

#include <boost/thread.hpp>

#include "central_queues.h"
#include "client_manager.h"

class LeaderManager {
public:
    LeaderManager(CentralQueues* queues, ClientManager* clientManager);

    bool GetLeaderAddress(sockaddr_in* addr); // Some silly c style interface
    bool is_curr_client_leader();
    bool is_leader(const sockaddr_in& addr);

    bool is_election_happening();

    void StartLeaderHeartbeat();
    void StopLeaderHeartBeat();

    void HandleElectionMessage(Payload msg);
    void ReceivedPing(Payload ping);
    void LeaderCrash();

    void UdpCrashDetected(const sockaddr_in& addr);
    std::vector<sockaddr_in> GetHigherOrderPingTargets();

private:
    boost::thread* heartbeatThread_;
    CentralQueues* queues_;                        // Central queues for inter-communication
    ClientManager* clientManager_;
    std::vector<sockaddr_in> higherOrderClients_;

    bool cancelledElection_ = false;
    bool electionInProgress_ = false;

    void StartElection();
    void HeartBeatPing();

    bool RemoveHigherOrderClient(const ClientInfo& info);
};


#endif //JAM_LEADER_MANAGER_H
