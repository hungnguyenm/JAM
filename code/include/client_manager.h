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

    // Return a list of all current clients
    std::vector<ClientInfo> GetAllClients();

    std::vector<sockaddr_in> GetAllClientSockAddressWithoutMe();

    std::vector<sockaddr_in> GetAllClientSockAddress();

    int get_client_count();

    // Return a list of higher order clients for election
    std::vector<ClientInfo> GetHigherOrderClients();
    std::vector<ClientInfo> GetHigherOrderClients(sockaddr_in client);
    std::vector<ClientInfo> GetHigherOrderClients(ClientInfo client);

    bool AddClient(ClientInfo client);
    bool AddClient(sockaddr_in client, const std::string &username, bool isLeader);

    bool RemoveClient(sockaddr_in client, std::string *username);
    bool RemoveClient(ClientInfo client, std::string *username);

    void RemoveAllClients();

    static std::string StringifyClient(sockaddr_in client);
    void PrintClients();

    // Encode/Decode methods
    static uint32_t EncodeSingleAddress(uint8_t *payload, sockaddr_in *addr);
    JamStatus EncodeClientList();

    static JamStatus DecodeSingleAddress(uint8_t *payload, uint32_t length, sockaddr_in *addr);
    JamStatus DecodeBufferToClientList(uint8_t *payload, uint32_t length);

    // Getters/Setters
    void set_self_address(sockaddr_in addr);
    sockaddr_in get_self_address();

    bool set_new_leader(const ClientInfo&  newLeader);
    ClientInfo* get_client_info(const ClientInfo& info);
    ClientInfo* get_current_leader();

    uint8_t *GetPayload();
    uint32_t GetPayloadSize();

private:
    sockaddr_in self_addr_;

    uint8_t encoded_data_[MAX_CLIENT_BUFFER_LENGTH];
    uint32_t encoded_data_size_;
    std::vector<ClientInfo> client_list_;
};


#endif //JAM_CLIENTMANAGER_H