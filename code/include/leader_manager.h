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

    ClientInfo* GetCurrentLeader();

    void StartLeaderHeartbeat();
    void StopLeaderHeartBeat();

    void HandleElectionMessagge(Payload msg);
    void ReceivedPing(Payload ping);
    void PingTimedOut();

    bool PingLeader();

private:
    boost::thread* heartbeatThread_;
    CentralQueues* queues_;                         // Central queues for inter-communication
    ClientManager* clientManager_;

    ClientInfo* lastLeader_;

    long sentElectionCandidatesOut_;

    void StartElection();
    void HeartBeatPing();

    boost::mutex m_leader_;
};


#endif //JAM_LEADER_MANAGER_H
