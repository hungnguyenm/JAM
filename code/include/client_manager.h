//
// Created by acharyab on 3/31/16.
//

#ifndef JAM_CLIENTMANAGER_H
#define JAM_CLIENTMANAGER_H

#include "client_info.h"
#include <vector>

///* Data Structs */
//struct client {
//    std::string username;
//    std::string client_ip_address;
//    std::string client_port;
//};

//TODO: client list is a vector of clients (which includes username, ip_address, client_port)?

class ClientManager {
    public:                    // begin public section
        ClientManager(ClientInfo client);     // constructor
        ~ClientManager();    // destructor
        std::vector<ClientInfo> GetAllClients(); //return a list of all current clients
        std::vector<ClientInfo> GetHigherOrderClients(sockaddr_in client); //return a list of higher order clients for election
        std::vector<ClientInfo> GetHigherOrderClients(ClientInfo client);
        void HandleNotification(); //status update
        void AddClient(sockaddr_in client);
        void AddClient(ClientInfo client);
        void RemoveClient(sockaddr_in client);
        void RemoveClient(ClientInfo client);
        void PrintClients();

    private:
        //sockaddr_in address_;
        std::vector<ClientInfo> client_list_;
        void HandleNewClient(); //deals with new clients joining the chat
        void HandleCrashClient(); //deals with existing clients crashing

};


#endif //JAM_CLIENTMANAGER_H
