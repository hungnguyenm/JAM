//
// Created by acharyab on 3/31/16.
//

#ifndef JAM_CLIENTMANAGER_H
#define JAM_CLIENTMANAGER_H

#include <vector>
#include "client_info.h"
#include "config.h"

//TODO: client list is a vector of clients (which includes username, ip_address, client_port)?

class ClientManager {
public:                    // begin public section
    ClientManager();

    ~ClientManager();    // destructor

    std::vector<ClientInfo> GetAllClients(); //return a list of all current clients

    std::vector<sockaddr_in> GetAllClientSockAddress();

    std::vector<ClientInfo> GetHigherOrderClients(
            sockaddr_in client); //return a list of higher order clients for election

    std::vector<ClientInfo> GetHigherOrderClients(ClientInfo client);

    void HandleNotification(); //status update

    void AddClient(sockaddr_in client, const std::string &username, bool isLeader);

    void AddClient(ClientInfo client);

    void RemoveClient(sockaddr_in client);

    void RemoveClient(ClientInfo client);


    void RemoveAllClients();

    JamStatus EncodeClientList();

    JamStatus DecodeBufferToClientList(uint8_t *payload, uint32_t length);

    void PrintClients();

    uint8_t *GetPayload();

    uint32_t GetPayloadSize();

private:
    enum {
        ENCODED_CLIENT_LENGTH = 6
    };

    uint8_t encoded_data_[MAX_CLIENT_BUFFER];
    uint32_t encoded_data_size_;
    std::vector<ClientInfo> client_list_;

    void HandleNewClient(); //deals with new clients joining the chat
    void HandleCrashClient(); //deals with existing clients crashing
};


#endif //JAM_CLIENTMANAGER_H
