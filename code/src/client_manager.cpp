//
// Author: Bipeen Acharya
// Created: 03/30/16
//
#include <iostream>
#include <vector>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <algorithm>

using namespace std;
#include "../include/client_manager.h"

// constructor of ClientManager,
ClientManager::ClientManager(ClientInfo client)
{
    client_list_.push_back(client);
}

ClientManager::~ClientManager()                 // destructor, just an example
{

}

vector<ClientInfo> ClientManager::GetAllClients() {
    return client_list_;
}

vector<ClientInfo> ClientManager::GetHigherOrderClients(sockaddr_in client) {
    return GetHigherOrderClients(ClientInfo(client));
}

vector<ClientInfo> ClientManager::GetHigherOrderClients(ClientInfo client) {
    int i;
    if (client_list_.size() > 0) {
        for (i=0; i<client_list_.size(); i++) {
            if (client < client_list_[i]) {
                return std::vector<client_list_.begin() + i, client_list_.end()>;
            }
        }
    }
}

void ClientManager::HandleNotification() {

}

void ClientManager::HandleNewClient() {

}

void ClientManager::HandleCrashClient() {


}

void ClientManager::AddClient(sockaddr_in client) {
    AddClient(ClientInfo(client));

}

void ClientManager::AddClient(ClientInfo client){
    client_list_.push_back(client);
    sort(client_list_.begin(), client_list_.end());
}

void ClientManager::RemoveClient(sockaddr_in client) {
    RemoveClient(ClientInfo(client));
}

void ClientManager::RemoveClient(ClientInfo client) {
    int i;
    if (client_list_.size() >0 ) {
        for (i=0;i<client_list_.size(); i++) {
            if (client == client_list_[i]) {
                client_list_.erase(client_list_.begin() + i);
            }
        }
    }
}

void ClientManager::PrintClients() {
    if (client_list_.size() >0) {
        for (int i=0; i< client_list_.size(); i++) {
            printf("BOBO");
        }
    }
}

