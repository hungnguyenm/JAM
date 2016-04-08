//
// Author: Bipeen Acharya
// Created: 03/30/16
//
#include <iostream>
#include <vector>

#include <arpa/inet.h>
#include <algorithm>

#include "../include/client_manager.h"

using namespace std;


// constructor of ClientManager,
ClientManager::ClientManager(sockaddr_in client) {
    client_list_.push_back(ClientInfo(client));
}


ClientManager::ClientManager() {

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
    vector<ClientInfo> higher_order_clients;
    if (client_list_.size() > 0) {
        for (i = 0; i < client_list_.size(); i++) {
            if (client < client_list_[i]) {
                higher_order_clients.push_back(client_list_[i]);
            }
        }
    }
    return higher_order_clients;
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

void ClientManager::AddClient(ClientInfo client) {
    client_list_.push_back(client);
    //TODO: sort doesnt work
//    sort(client_list_.begin(), client_list_.end());
}

void ClientManager::RemoveClient(sockaddr_in client) {
    RemoveClient(ClientInfo(client));
}

void ClientManager::RemoveClient(ClientInfo client) {
    int i;
    if (client_list_.size() > 0) {
        for (i = 0; i < client_list_.size(); i++) {
            if (client == client_list_[i]) {
                client_list_.erase(client_list_.begin() + i);
            }
        }
    }
}

JamStatus ClientManager::EncodeClientList(uint8_t *payload, uint32_t *length){
    uint8_t *buffer = payload;
    *length = 0;
    JamStatus ret;

    if (client_list_.size() * ENCODED_CLIENT_LENGTH > CLIENT_EXCEED_MAXIMUM) {
        ret = CLIENT_EXCEED_MAXIMUM;
    } else {
        if (client_list_.size() > 0) {
            for (int i = 0; i < client_list_.size(); i++) {
                *length += packu32(buffer, client_list_[i].GetSockAddress().sin_addr.s_addr);
                *length += packu16(buffer, client_list_[i].GetSockAddress().sin_port);
            }
        }
        ret = SUCCESS;
    }
    return ret;
}

JamStatus ClientManager::DecodeClientList(uint8_t *payload, uint32_t length){
    JamStatus ret;
    uint8_t *buffer = payload;

    sockaddr_in myaddr;

    if (length % 6 != 0) {
        ret = BUFFER_INVALID_LENGTH;
    } else {
        for (int i=0; i<length; i+=ENCODED_CLIENT_LENGTH) {
            myaddr.sin_family = AF_INET;
            myaddr.sin_addr.s_addr = unpacku32(buffer);
            myaddr.sin_port = unpacku16(buffer);
            client_list_.push_back(ClientInfo(myaddr));
        }
        ret = SUCCESS;
    }
    return ret;
}

void ClientManager::PrintClients() {
    if (client_list_.size() > 0) {
        for (int i = 0; i < client_list_.size(); i++) {
            printf("%s\n", inet_ntoa(client_list_[i].GetSockAddress().sin_addr));
        }
    }
}

uint32_t ClientManager::packu32(uint8_t *&buf, uint32_t i) {
    uint32_t n = htonl(i);
    *buf++ = n >> 24;
    *buf++ = n >> 16;
    *buf++ = n >> 8;
    *buf++ = n;
    return 4;
}

uint16_t ClientManager::packu16(uint8_t *&buf, uint16_t i) {
    uint16_t n = htons(i);
    *buf++ = n >> 8;
    *buf++ = n;
    return 2;
}

uint32_t ClientManager::unpacku32(uint8_t *&buf) {
    uint32_t ui = ((uint32_t) buf[0] << 24) |
                  ((uint32_t) buf[1] << 16) |
                  ((uint32_t) buf[2] << 8) |
                  buf[3];
    buf += 4;
    uint32_t un = ntohl(ui);
    return un;
}


uint16_t ClientManager::unpacku16(uint8_t *&buf) {
    uint16_t ui = ((uint32_t) buf[2] << 8) |
                  buf[3];
    buf += 2;
    uint16_t un = ntohs(ui);
    return un;
}

