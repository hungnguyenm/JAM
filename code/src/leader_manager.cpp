//
// Created by Krzysztof Jordan on 4/9/16.
//

#include "../include/leader_manager.h"

LeaderManager::LeaderManager(CentralQueues* queues, ClientManager* clientManager) :
        queues_(queues), clientManager_(clientManager) {

}

ClientInfo* LeaderManager::GetCurrentLeader() {
    boost::mutex::scoped_lock lock(m_leader_);

    if (lastLeader_ == nullptr) {
        lastLeader_ = clientManager_->get_current_leader();
    }

    return lastLeader_;
}

bool LeaderManager::GetLeaderAddress(sockaddr_in* addr) {
    ClientInfo* leader = GetCurrentLeader();

    if(leader != nullptr && addr != nullptr) {
        *addr = leader->get_sock_address();
        return true;
    }

    return false;
}


bool LeaderManager::is_curr_client_leader() {
    ClientInfo* leader = GetCurrentLeader();

    return (leader != nullptr) ? (*leader == clientManager_->get_self_address()) : false;
}

bool LeaderManager::is_election_happening() {
    return electionInProgress_;
}


void LeaderManager::ReceivedPing(Payload ping) {
    boost::mutex::scoped_lock lock(m_leader_);

    ClientInfo* leader = GetCurrentLeader();

    if(*leader == clientManager_->get_self_address()) {
        // Ping back the client since you are the leader
        queues_->push(CentralQueues::LEADER_OUT, ping);
    }
}

bool LeaderManager::PingLeader() {
    boost::mutex::scoped_lock lock(m_leader_);

    ClientInfo* leader = GetCurrentLeader();

    if(leader == nullptr) {
        DCOUT("Election in progress, no heartbeat");
        return false;
    }

    Payload payload;
    payload.SetType(MessageType::STATUS_MSG);
    payload.SetStatus(Status::PING);

    sockaddr_in addr =  leader->get_sock_address();
    payload.SetAddress(&addr);

    queues_->push(CentralQueues::QueueType::LEADER_OUT, payload);
    return true;
}

void LeaderManager::PingTimedOut() {
    StartElection();
}

void LeaderManager::StartElection() {
    boost::mutex::scoped_lock lock(m_leader_);
    electionInProgress_ = true;
    cancelledElection_ = false;

    StopLeaderHeartBeat();

    auto higherOrderClients = clientManager_->GetHigherOrderClients();
    sentElectionCandidatesOut_ = higherOrderClients.size();

    Payload payload;
    payload.SetType(MessageType::ELECTION_MSG);

    // Declare yourself to be the winner and tell all clients that you are the winner
    if (higherOrderClients.size() == 0) {
        auto clients = clientManager_->GetAllClients();
        payload.SetElectionCommand(ElectionCommand::ELECT_WIN);

        for(int i = 0; i < clients.size(); i++) {
            sockaddr_in  addr = clients[i].get_sock_address();
            payload.SetAddress(&addr);

            queues_->push(CentralQueues::LEADER_OUT, payload);
        }
        return;
    }

    // tell each higher order client that you want to declare yourself to be a leader
    for (int i = 0; i < higherOrderClients.size(); i++) {
        payload.SetElectionCommand(ElectionCommand::ELECT_CANDIDATE);

        for(int i = 0; i < higherOrderClients.size(); i++) {
            sockaddr_in addr = higherOrderClients[i].get_sock_address();
            payload.SetAddress(&addr);

            queues_->push(CentralQueues::LEADER_OUT, payload);
        }
    }
}


void LeaderManager::HandleElectionMessage(Payload msg)
{
    boost::mutex::scoped_lock lock(m_leader_);

    ClientInfo selfInfo = ClientInfo(clientManager_->get_self_address());
    auto clients = clientManager_->GetAllClients();

    switch(msg.GetElectionCommand()) {
        case ElectionCommand::ELECT_START:
            StartElection();
            break;

        case ElectionCommand::ELECT_CANDIDATE:
            // check if the client that sent you the message is bigger
            if(selfInfo < *msg.GetAddress()) {
                msg.SetElectionCommand(ElectionCommand::ELECT_YIELD);
                queues_->push(CentralQueues::LEADER_OUT, msg);
            } else {
                msg.SetElectionCommand(ElectionCommand::ELECT_STOP);
                queues_->push(CentralQueues::LEADER_OUT, msg);
                StartElection();
            }

            break;

        case ElectionCommand::ELECT_STOP:
            sentElectionCandidatesOut_ = 0;
            cancelledElection_ = true;
            break;

        case ElectionCommand::ELECT_YIELD:
            --sentElectionCandidatesOut_;

            if(sentElectionCandidatesOut_ > 0 || cancelledElection_) {
                break;
            }

            msg.SetElectionCommand(ELECT_WIN);

            for(int i = 0; i < clients.size(); i++) {
                sockaddr_in addr = clients[i].get_sock_address();
                msg.SetAddress(&addr);

                queues_->push(CentralQueues::LEADER_OUT, msg);
            }

            clientManager_->set_new_leader(clientManager_->get_self_address());
            break;

        case ElectionCommand::ELECT_WIN:
            // Set the new leader here
            electionInProgress_ = false;
            clientManager_->set_new_leader(*msg.GetAddress());
            break;

    }
}

void LeaderManager::StartLeaderHeartbeat() {
    boost::mutex::scoped_lock lock(m_leader_);

    StopLeaderHeartBeat();

    heartbeatThread_ = new boost::thread(boost::bind(&LeaderManager::HeartBeatPing, this));
}

void LeaderManager::HeartBeatPing() {
    while(true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(PING_INTERVAL));
        PingLeader();
    }
}

void LeaderManager::StopLeaderHeartBeat() {
    boost::mutex::scoped_lock lock(m_leader_);

    if(heartbeatThread_ != nullptr) {
        heartbeatThread_->interrupt();
        delete heartbeatThread_;
    }
}
