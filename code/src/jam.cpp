/**
 * JAM class - Module Coordinator acts as the link between all internal modules.
 * It functions as the single point of contact of JAM.
 *
 * @author: Hung Nguyen
 * @version 1.0 04/07/16
 */

#include "../include/jam.h"

#ifdef STRESS
#include "../include/stress_tester.h"
#endif

#include <ifaddrs.h>
#include <arpa/inet.h>

using namespace std;

JAM::JAM()
        : udpWrapper_(&queues_),
          userHandler_(&queues_),
          leaderManager_(&queues_, &clientManager_),
          holdQueue_(&queues_),
          order_(DEFAULT_FIRST_ORDER),
          last_witness_order_(DEFAULT_FIRST_ORDER - 1) {
    holdQueue_.SetUserHandlerPipe(userHandler_.get_write_pipe());
}

JAM::~JAM() {

}

void JAM::StartAsLeader(const char *user_name,
                        const char *user_interface,
                        const char *user_port) {
    // INFO
    cout << user_name << " is starting a new chat group!" << endl;
    user_name_ = user_name;

    // Start UDP Wrapper
    uint16_t bind_port;
    sockaddr_in servaddr;
    if (udpWrapper_.Start(user_port, &bind_port) == SUCCESS) {
        // Detect interface address
        if (GetInterfaceAddress(user_interface, bind_port, &servaddr)) {
            // Add creator as leader
            clientManager_.AddClient(servaddr, user_name, true);
            clientManager_.set_self_address(servaddr);
        } else {
            cerr << "Failed to detect network interface!" << endl;
            exit(1);
        }

        cout << "Succeeded, listening on " << clientManager_.StringifyClient(servaddr) <<
        ". Current users:" << endl;
        clientManager_.PrintClients();
    } else {
        cerr << "Failed to start new chat group!" << endl;
        exit(1);
    }

    // Start all other modules
    userHandler_.Start();
    leaderManager_.StartLeaderHeartbeat();

    // Start-up completed
    cout << "Waiting for others to join..." << endl;

    Main();
}

void JAM::StartAsClient(const char *user_name,
                        const char *user_interface,
                        const char *user_port,
                        const char *serv_addr,
                        const char *serv_port) {
    // INFO
    cout << user_name << " is joining a chat group at " << serv_addr << ":" << serv_port << "!" << endl;
    user_name_ = user_name;

    // Start UDP Wrapper
    uint16_t bind_port;
    sockaddr_in client_addr;
    if (udpWrapper_.Start(user_port, &bind_port) == SUCCESS) {
        // Detect interface address
        if (GetInterfaceAddress(user_interface, bind_port, &client_addr)) {
            clientManager_.set_self_address(client_addr);
        } else {
            cerr << "Failed to detect network interface!" << endl;
            exit(1);
        }

        cout << "Listening on " << clientManager_.StringifyClient(client_addr) << endl;
    } else {
        cerr << "Failed to start UDP service!" << endl;
        exit(1);
    }

    // Initiate hand-shake
    Payload payload;
    payload.SetType(STATUS_MSG);
    payload.SetStatus(CLIENT_JOIN);
    payload.SetUsername(user_name);

    sockaddr_in addr;
    if (udpWrapper_.GetAddressFromInfo(serv_addr, serv_port, &addr) == SUCCESS) {
        if (payload.EncodePayload() == SUCCESS) {
            udpWrapper_.SendPayloadSingle(payload, &addr);
        } else {
            cerr << "Internal error! Exiting..." << endl;
            exit(1);
        }

    } else {
        cerr << "Invalid server address!" << endl;
        exit(1);
    }

    if (queues_.wait_for_data(JOIN_TIMEOUT)) {
        // Only check incoming UDP queue
        if (queues_.try_pop_udp_in(payload)) {
            if (payload.GetType() == STATUS_MSG && payload.GetStatus() == CLIENT_JOIN_ACK) {
                if (clientManager_.DecodeBufferToClientList((uint8_t *) payload.GetMessage().c_str(),
                                                            payload.GetMessageLength()) == SUCCESS) {
                    // Add self
                    clientManager_.AddClient(client_addr, user_name, false);
                    goto next;
                } else {
                    cerr << "Failed to hand-shake with server!" << endl;
                    exit(1);
                }
            }
        }
        goto exit;

    } else {
        goto exit;
    }

    exit:
    // UDP timeout for hand-shake or crashed
    udpWrapper_.Stop();
    cout << "Sorry, no chat is active on " << serv_addr << ":" <<
    serv_port << ", try again later." << endl;
    cout << "Bye." << endl;
    exit(0);

    next:
    // Start all other modules
    userHandler_.Start();
    leaderManager_.StartLeaderHeartbeat();

    // Start-up completed
    cout << "Succeeded. Current users:" << endl;
    clientManager_.PrintClients();

    // Notify all other clients
    payload.clear();
    payload.SetType(STATUS_MSG);
    payload.SetStatus(CLIENT_JOIN_MULTICAST);
    payload.SetUsername(user_name);

    vector<sockaddr_in> list = clientManager_.GetAllClientSockAddressWithoutMe();
    udpWrapper_.SendPayloadList(payload, &list);

    Main();
}

void JAM::Main() {
    Payload payload;
    sockaddr_in addr;
    int32_t history_request;
    vector<sockaddr_in> multicast_list;
    string username;
    uint8_t buffer[MAX_MESSAGE_LENGTH];
    uint32_t length;

#ifdef STRESS
    StressTester stressTester(&queues_, STRESS_TEST_INTERVAL, "test_messages.txt");
#endif

    // Infinite loop to monitor central communication
    for (; ;) {
        if (queues_.wait_for_data(JAM_CENTRAL_TIMEOUT)) {
            if (queues_.is_terminate()) {
                DCOUT("INFO: JAM - Received terminate signal");
                break;          // Only exit loop if receive terminate signal
            }

            // Go through each queue and handle data if available
            bool has_data;
            do {
                has_data = false;
                if (!queues_.is_empty(CentralQueues::QueueType::USER_OUT) &&
                    !leaderManager_.is_election_happening() &&
                    leaderManager_.GetLeaderAddress(&addr)) {
                    // These conditions are for reducing computation overhead in case no leader
                    if (queues_.try_pop_user_out(payload)) {
                        has_data = true;
                        payload.SetType(CHAT_MSG);
                        payload.SetUsername(user_name_);
                        if (payload.EncodePayload() == SUCCESS) {
                            udpWrapper_.SendPayloadSingle(payload, &addr);
                        }
                    }
                }

                if (queues_.try_pop_udp_crash(addr)) {
                    has_data = true;
                    if (clientManager_.RemoveClient(addr, &username)) {
                        DCOUT("INFO: JAM - Client unreachable at " +
                              ClientManager::StringifyClient(addr));
                        cout << "NOTICE - " << username << " crashed." << endl;

                        // Clear history
                        udpWrapper_.ClearReceivedHistory(&addr);

                        // Rebuild payload to notify all
                        payload.clear();
                        payload.SetType(STATUS_MSG);
                        payload.SetStatus(CLIENT_CRASH);
                        length = ClientManager::EncodeSingleAddress(buffer, &addr);
                        payload.SetMessage(buffer, length);

                        multicast_list = clientManager_.GetAllClientSockAddressWithoutMe();
                        udpWrapper_.SendPayloadList(payload, &multicast_list);
                    }
                    leaderManager_.UdpCrashDetected(addr);
                }

                if (queues_.try_pop_udp_in(payload)) {
                    // TODO: implement handler for all types of payload
                    has_data = true;
                    switch (payload.GetType()) {
                        case CHAT_MSG:
                            if (payload.GetOrder() == DEFAULT_NO_ORDER && leaderManager_.is_curr_client_leader()) {
                                // Need to handle ordering
                                payload.SetOrder(order_);
                                multicast_list = clientManager_.GetAllClientSockAddress();
                                udpWrapper_.SendPayloadList(payload, &multicast_list);
                                order_++;
                            } else {
                                last_witness_order_ = payload.GetOrder();
                                holdQueue_.AddMessageToQueue(payload);
                            }
                            break;

                        case STATUS_MSG:
                            switch (payload.GetStatus()) {
                                case CLIENT_JOIN:
                                    if (leaderManager_.is_election_happening()) {
                                        // There is an election going on, need to defer this client
                                        joinQueue_.push(payload);
                                    } else {
                                        // Rebuild payload to acknowledge
                                        payload.clear();
                                        payload.SetType(STATUS_MSG);
                                        payload.SetStatus(CLIENT_JOIN_ACK);
                                        payload.SetMessage(clientManager_.GetPayload(),
                                                           clientManager_.GetPayloadSize());

                                        addr = *payload.GetAddress();
                                        udpWrapper_.SendPayloadSingle(payload, &addr);
                                    }
                                    break;
                                case CLIENT_JOIN_MULTICAST:
                                    addr = *payload.GetAddress();
                                    if (clientManager_.AddClient(addr, payload.GetUsername(), false)) {
                                        cout << "NOTICE - " << payload.GetUsername() << " joined on " <<
                                        clientManager_.StringifyClient(addr) << "." << endl;
                                    }
                                    break;
                                case CLIENT_LEAVE:
                                    addr = *payload.GetAddress();
                                    if (clientManager_.RemoveClient(addr, &username)) {
                                        cout << "NOTICE - " << username << " left the chat." << endl;
                                    }
                                    udpWrapper_.ClearReceivedHistory(&addr);
                                    break;
                                case CLIENT_CRASH:
                                    if (ClientManager::DecodeSingleAddress((uint8_t *) payload.GetMessage().c_str(),
                                                                           payload.GetMessageLength(),
                                                                           &addr) == SUCCESS) {
                                        if (clientManager_.RemoveClient(addr, &username)) {
                                            cout << "NOTICE - " << username << " crashed." << endl;
                                        }
                                        udpWrapper_.ClearReceivedHistory(&addr);
                                    } else {
                                        DCERR("ERROR: JAM - Failed to decode sockaddr_in of crash info.");
                                    }
                                    break;
                                case LEADER_LEAVE:
                                    addr = *payload.GetAddress();
                                    if (clientManager_.RemoveClient(addr, &username)) {
                                        cout << "NOTICE - " << username << " left the chat." << endl;
                                    }
                                    leaderManager_.UdpCrashDetected(addr);
                                    udpWrapper_.ClearReceivedHistory(&addr);
                                    break;
                                default:
                                    break;
                            }
                            break;

                        case ELECTION_MSG:
                            leaderManager_.HandleElectionMessage(payload);
                            if (payload.GetElectionCommand() == ELECT_WIN) {
                                addr = *payload.GetAddress();
                                udpWrapper_.LeaderRecover(&addr);

                                // Handle deferred joining client
                                while (joinQueue_.try_pop(payload)) {
                                    // Rebuild payload to acknowledge
                                    payload.clear();
                                    payload.SetType(STATUS_MSG);
                                    payload.SetStatus(CLIENT_JOIN_ACK);
                                    payload.SetMessage(clientManager_.GetPayload(),
                                                       clientManager_.GetPayloadSize());

                                    addr = *payload.GetAddress();
                                    udpWrapper_.SendPayloadSingle(payload, &addr);
                                }
                            }
                            break;

                        case RECOVER_MSG:
                            switch (payload.GetRecoverCommand()) {
                                case MSG_LOST:
                                    addr = *payload.GetAddress();
                                    history_request = payload.GetOrder();
                                    if (holdQueue_.GetPayloadInHistory(history_request, &payload)) {
                                        udpWrapper_.SendPayloadSingle(payload, &addr);
                                    }
                                    break;
                            }
                            break;

                        default:
                            break;
                    }
                }

                if (queues_.try_pop_leader_out(payload)) {
                    has_data = true;
                    switch (payload.GetType()) {
                        case STATUS_MSG:
                            if (payload.GetStatus() == PING) {
                                if (leaderManager_.is_curr_client_leader()) {
                                    // This client is the leader, need to ping all other clients
                                    multicast_list = clientManager_.GetAllClientSockAddressWithoutMe();
                                    udpWrapper_.SendPayloadList(payload, &multicast_list);
                                    udpWrapper_.LeaderRecover();
                                } else if (leaderManager_.GetLeaderAddress(&addr)) {
                                    // This client is not the leader, need to ping the leader
                                    udpWrapper_.SendPayloadSingle(payload, &addr);
                                    udpWrapper_.LeaderRecover(&addr);
                                }
                            } else if (payload.GetStatus() == PING_TARGET) {
                                // Handle targeted ping
                                multicast_list = leaderManager_.GetHigherOrderPingTargets();
                                udpWrapper_.SendPayloadList(payload, &multicast_list);
                            }
                            break;
                        case ELECTION_MSG:
                            if (payload.GetElectionCommand() == ELECT_WIN) {
                                // Distribute to all
                                multicast_list = clientManager_.GetAllClientSockAddressWithoutMe();
                                udpWrapper_.SendPayloadList(payload, &multicast_list);
                                // Do recovery
                                order_ = last_witness_order_ + 1;
                                addr = clientManager_.get_self_address();
                                udpWrapper_.LeaderRecover(&addr);
                            } else {
                                // Targeted payload
                                udpWrapper_.SendPayloadSingle(payload, payload.GetAddress());
                            }
                            break;
                        default:
                            break;
                    }
                }

                if (!queues_.is_empty(CentralQueues::QueueType::HISTORY_REQUEST) &&
                    !leaderManager_.is_election_happening() &&
                    leaderManager_.GetLeaderAddress(&addr)) {
                    // These conditions are for reducing computation overhead in case no leader
                    if (queues_.try_pop_history_request(history_request)) {
                        has_data = true;
                        payload.SetType(RECOVER_MSG);
                        payload.SetRecoverCommand(MSG_LOST);
                        payload.SetOrder(history_request);
                        if (payload.EncodePayload() == SUCCESS) {
                            udpWrapper_.SendPayloadSingle(payload, &addr);
                        }
                    }
                }

            } while (has_data);
        }
    }

    // Terminating
    payload.clear();
    payload.SetType(STATUS_MSG);
    if (leaderManager_.is_curr_client_leader()) {
        payload.SetStatus(LEADER_LEAVE);
    } else {
        payload.SetStatus(CLIENT_LEAVE);
    }
    multicast_list = clientManager_.GetAllClientSockAddressWithoutMe();
    udpWrapper_.SendPayloadList(payload, &multicast_list);

    udpWrapper_.Stop();
    leaderManager_.StopLeaderHeartBeat();

    cout << "Bye." << endl;
}

string JAM::GetInterfaceAddressStr(const char *port) {
    stringstream ss;
    ifaddrs *ifap, *ifa;
    sockaddr_in *sa;                // Only print IPv4
    char *name, *addr;
    const char *sep = "";

    getifaddrs(&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family == AF_INET) {
            name = ifa->ifa_name;
            if (strcmp(name, "lo0") != 0) {     // Ignore loop-back interface
                sa = (sockaddr_in *) ifa->ifa_addr;
                addr = inet_ntoa(sa->sin_addr);
                ss << sep << " (" << (const char *) name << ")" << (const char *) addr << ":" << port;
                sep = ",";
            }
        }
    }

    return ss.str();
}

string JAM::GetInterfaceAddressStr(const char *interface, const char *port) {
    stringstream ss;
    ifaddrs *ifap, *ifa;
    sockaddr_in *sa;                // Only print IPv4
    char *name, *addr;
    const char *sep = "";

    getifaddrs(&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family == AF_INET) {
            name = ifa->ifa_name;
            if (strcmp(name, interface) == 0) {     // Only select this interface
                sa = (sockaddr_in *) ifa->ifa_addr;
                addr = inet_ntoa(sa->sin_addr);
                ss << sep << " (" << (const char *) name << ")" << (const char *) addr << ":" << port;
                sep = ",";
            }
        }
    }

    return ss.str();
}

bool JAM::GetInterfaceAddress(const char *interface, uint16_t port, sockaddr_in *addr) {
    bool ret = false;
    ifaddrs *ifap, *ifa;
    char *name;

    getifaddrs(&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family == AF_INET) {
            name = ifa->ifa_name;
            if (strcmp(name, interface) == 0) {     // Only select this interface
                ret = true;
                memcpy(addr, ifa->ifa_addr, sizeof(sockaddr_in));
                addr->sin_port = port;
            }
        }
    }

    return ret;
}

void JAM::PrintClientList() {
    cout << "Current users:" << endl;
    clientManager_.PrintClients();
}
