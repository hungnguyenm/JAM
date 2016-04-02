//
// Created by acharyab on 3/31/16.
//

#ifndef JAM_CLIENTMANAGER_H
#define JAM_CLIENTMANAGER_H


class ClientManager {
    char *string;
    std::vector<sockaddr_in> _client_list;


    public:                    // begin public section
        ClientManager();     // constructor
        ~ClientManager();    // destructor
        std::vector<sockaddr_in> GetAllClients(); //return a list of all current clients
        std::vector<sockaddr_in> GetHigherOrderClients(); //return a list of higher order clients for election
        void HandleNotification(); //status update
        void AddClient();
        void RemoveClient();

    private:
        void HandleNewClient(); //deals with new clients joining the chat
        void HandleCrashClient(); //deals with existing clients crashing

};


#endif //JAM_CLIENTMANAGER_H
