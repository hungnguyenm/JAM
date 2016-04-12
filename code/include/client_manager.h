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

    void SetSelfAddress(sockaddr_in addr);

    // Return a list of all current clients
    std::vector<ClientInfo> GetAllClients();

    std::vector<sockaddr_in> GetAllClientSockAddressWithoutMe();

    std::vector<sockaddr_in> GetAllClientSockAddress();

    // Return a list of higher order clients for election
    std::vector<ClientInfo> GetHigherOrderClients(sockaddr_in client);

    std::vector<ClientInfo> GetHigherOrderClients(ClientInfo client);

    bool AddClient(sockaddr_in client, const std::string &username, bool isLeader);

    bool AddClient(ClientInfo client);

    static std::string PrintSingleClientIP(sockaddr_in client);

    bool RemoveClient(sockaddr_in client, std::string *username);

    bool RemoveClient(ClientInfo client, std::string *username);

    void RemoveAllClients();

    JamStatus EncodeClientList();

    JamStatus DecodeBufferToClientList(uint8_t *payload, uint32_t length);

    void PrintClients();

    uint8_t *GetPayload();

    uint32_t GetPayloadSize();

    static uint32_t EncodeSingleAddress(uint8_t *payload, sockaddr_in *addr);

    static JamStatus DecodeSingleAddress(uint8_t *payload, uint32_t length, sockaddr_in *addr);

private:
    sockaddr_in self_addr_;

    uint8_t encoded_data_[MAX_CLIENT_BUFFER_LENGTH];
    uint32_t encoded_data_size_;
    std::vector<ClientInfo> client_list_;
};


#endif //JAM_CLIENTMANAGER_H
