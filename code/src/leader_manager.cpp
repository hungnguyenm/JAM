//
// Created by Krzysztof Jordan on 4/9/16.
//

#include "../include/leader_manager.h"

LeaderManager::LeaderManager(CentralQueues *queues, ClientManager *clientManager) :
        queues_(queues), clientManager_(clientManager) {
}

bool LeaderManager::is_leader(const sockaddr_in &addr) {
    ClientInfo *leader = clientManager_->get_client_info(addr);

    return (leader == nullptr) ? false : leader->is_leader();
}

bool LeaderManager::GetLeaderAddress(sockaddr_in *addr) {
    ClientInfo *leader = clientManager_->get_current_leader();

    if (leader != nullptr && addr != nullptr) {
        *addr = leader->get_sock_address();
        return true;
    }

    return false;
}

bool LeaderManager::is_curr_client_leader() {
    ClientInfo *leader = clientManager_->get_current_leader();

    return (leader != nullptr) ? (*leader == clientManager_->get_self_address()) : false;
}

bool LeaderManager::is_election_happening() {
    return electionInProgress_;
}

void LeaderManager::ReceivedPing(Payload ping) {
    ClientInfo *leader = clientManager_->get_current_leader();

    if (*leader == clientManager_->get_self_address()) {
        // Ping back the client since you are the leader
        queues_->push(CentralQueues::LEADER_OUT, ping);
    }
}

bool LeaderManager::Ping() {
    Payload payload;
    payload.SetType(MessageType::STATUS_MSG);
    payload.SetStatus(Status::PING);

    queues_->push(CentralQueues::QueueType::LEADER_OUT, payload);
    return true;
}

void LeaderManager::LeaderCrash() {
    StartElection();
}

void LeaderManager::StartElection() {
    DCOUT("INFO: LM - Start election");

    if (electionInProgress_) {
        return;
    }

    electionInProgress_ = true;
    cancelledElection_ = false;

    auto higherOrderClients = clientManager_->GetHigherOrderClients();
    sentElectionCandidatesOut_ = higherOrderClients.size();

    Payload payload;
    payload.SetType(MessageType::ELECTION_MSG);

    // Declare yourself to be the winner and tell all clients that you are the winner
    if (higherOrderClients.size() == 0) {
        if(clientManager_->get_client_count() > 1) {
            payload.SetElectionCommand(ElectionCommand::ELECT_WIN);
            queues_->push(CentralQueues::LEADER_OUT, payload);
        }
        clientManager_->set_new_leader(clientManager_->get_self_address());

        return;
    }

    // tell each higher order client that you want to declare yourself to be a leader
    for (int i = 0; i < higherOrderClients.size(); i++) {
        payload.SetElectionCommand(ElectionCommand::ELECT_CANDIDATE);

        for (int i = 0; i < higherOrderClients.size(); i++) {
            sockaddr_in addr = higherOrderClients[i].get_sock_address();
            payload.SetAddress(&addr);

            queues_->push(CentralQueues::LEADER_OUT, payload);
        }
    }
}

void LeaderManager::HandleElectionMessage(Payload msg) {
    ClientInfo selfInfo = ClientInfo(clientManager_->get_self_address());

    switch (msg.GetElectionCommand()) {
        case ElectionCommand::ELECT_START:
            DCOUT("INFO: LM - election start received");
            StartElection();
            break;

        case ElectionCommand::ELECT_CANDIDATE:
            DCOUT("INFO: LM - Election candidate gotten");
            // check if the client that sent you the message is bigger
            if (selfInfo < *msg.GetAddress()) {
                msg.SetElectionCommand(ElectionCommand::ELECT_YIELD);
                queues_->push(CentralQueues::LEADER_OUT, msg);
            } else {
                msg.SetElectionCommand(ElectionCommand::ELECT_STOP);
                queues_->push(CentralQueues::LEADER_OUT, msg);
                StartElection();
            }

            break;

        case ElectionCommand::ELECT_STOP:
            DCOUT("INFO: LM - election stop received");
            sentElectionCandidatesOut_ = 0;
            cancelledElection_ = true;
            break;

        case ElectionCommand::ELECT_YIELD:
            DCOUT("INFO: LM - election yield received");
            --sentElectionCandidatesOut_;

            if (sentElectionCandidatesOut_ > 0 || cancelledElection_) {
                break;
            }

            DCOUT("INFO: LM - I won the election");
            msg.SetElectionCommand(ELECT_WIN);
            queues_->push(CentralQueues::LEADER_OUT, msg);
            clientManager_->set_new_leader(clientManager_->get_self_address());
            break;

        case ElectionCommand::ELECT_WIN:
            // Set the new leader here
            electionInProgress_ = false;
            clientManager_->set_new_leader(*msg.GetAddress());
            DCOUT("INFO: LM - Another client wins election");
            break;

    }
}

void LeaderManager::StartLeaderHeartbeat() {
    DCOUT("INFO: LM - Heartbeat started");

    heartbeatThread_ = new boost::thread(boost::bind(&LeaderManager::HeartBeatPing, this));
}

void LeaderManager::HeartBeatPing() {
    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(PING_INTERVAL));
        if (electionInProgress_ == false) {
            Ping();
        }
    }
}

void LeaderManager::StopLeaderHeartBeat() {
    if (heartbeatThread_ != nullptr) {
        heartbeatThread_->interrupt();
        delete heartbeatThread_;
        DCOUT("INFO: LM - Heartbeat stopped");
    }
}
