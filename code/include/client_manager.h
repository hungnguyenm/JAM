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
    ClientManager(sockaddr_in client);     // constructor
    ClientManager();

    ~ClientManager();    // destructor
    std::vector<ClientInfo> GetAllClients(); //return a list of all current clients
    std::vector<ClientInfo> GetHigherOrderClients(
            sockaddr_in client); //return a list of higher order clients for election
    std::vector<ClientInfo> GetHigherOrderClients(ClientInfo client);

    void HandleNotification(); //status update

    void AddClient(sockaddr_in client);

    void AddClient(ClientInfo client);

    void RemoveClient(sockaddr_in client);

    void RemoveClient(ClientInfo client);

    JamStatus EncodeClientList(uint8_t *payload, uint32_t *length);

    JamStatus DecodeClientList(uint8_t *payload, uint32_t length);

    void PrintClients();

private:
    enum {
        ENCODED_CLIENT_LENGTH = 6
    };

    std::vector<ClientInfo> client_list_;

    void HandleNewClient(); //deals with new clients joining the chat
    void HandleCrashClient(); //deals with existing clients crashing

    uint32_t packu32(uint8_t *&buf, uint32_t i);

    uint16_t packu16(uint8_t *&buf, uint16_t i);

    uint32_t unpacku32(uint8_t *&buf);

    uint16_t unpacku16(uint8_t *&buf);

};


#endif //JAM_CLIENTMANAGER_H
