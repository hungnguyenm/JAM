//
// Author: Bipeen Acharya
// Created: 03/30/16
//
#include <iostream>
#include <vector>

#include <arpa/inet.h>
#include <cstring>

#include "../include/client_manager.h"

using namespace std;

ClientManager::ClientManager() {

}

ClientManager::~ClientManager()                 // destructor
{

}

vector<ClientInfo> ClientManager::GetAllClients() {
    return client_list_;
}

std::vector<sockaddr_in> ClientManager::GetAllClientSockAddress() {
    std::vector<sockaddr_in> vectorOfSockAddress;
    if (client_list_.size() > 0) {
        for (int i = 0; i < client_list_.size(); i++) {
            vectorOfSockAddress.push_back(client_list_[i].GetSockAddress());
        }
    }
    return vectorOfSockAddress;
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

void ClientManager::AddClient(sockaddr_in client, const std::string &username, bool isLeader) {
    AddClient(ClientInfo(client, username, isLeader));
}

void ClientManager::AddClient(ClientInfo client) {
    client_list_.push_back(client);
    //TODO: sort doesnt work
    EncodeClientList();
//    sort(client_list_.begin(), client_list_.end());
}

//void ClientManager::RemoveClient(sockaddr_in client) {
//    RemoveClient(ClientInfo(client));
//}

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

void ClientManager::RemoveAllClients() {
    client_list_.clear();
}

JamStatus ClientManager::EncodeClientList() {
    JamStatus ret = SUCCESS;

    uint32_t L = ClientInfo::GetPacketSize();
    uint32_t target = L * client_list_.size();

    encoded_data_size_ = target;

    for (int i = 0; i < client_list_.size(); i++) {
        ClientInfo::EncodeClientInBuffer(client_list_[i], encoded_data_ + i * L);
    }
    return ret;
}

JamStatus ClientManager::DecodeBufferToClientList(uint8_t *payload, uint32_t length) {
    JamStatus ret = SUCCESS;

    uint8_t *buffer = payload;
    encoded_data_size_ = length;

    uint32_t username_length_;
    sockaddr_in myaddr;
    std::string username;

    uint32_t L = ClientInfo::GetPacketSize();

    if (length % L != 0) {
        ret = BUFFER_INVALID_LENGTH;
    } else {
        for (int i = 0; i < length; i += L) {
            username_length_ = SerializerHelper::unpacku32(buffer);
            if (username_length_ < MAX_USER_NAME_LENGTH) {
                memcpy((void *) username.c_str(), buffer, username_length_);
                buffer += username_length_;
            }

            myaddr.sin_family = AF_INET;
            myaddr.sin_addr.s_addr = SerializerHelper::unpacku32(buffer);
            myaddr.sin_port = SerializerHelper::unpacku16(buffer);

            bool isLeader = SerializerHelper::unpacku8(buffer);
            client_list_.push_back(ClientInfo(myaddr, username, isLeader));
        }
    }


    return ret;
}

void ClientManager::PrintClients() {
    if (client_list_.size() > 0) {
        for (int i = 0; i < client_list_.size(); i++) {
            printf("%s ", client_list_[i].get_username().c_str());
            printf("%s ", inet_ntoa(client_list_[i].GetSockAddress().sin_addr));
            printf("%c\n", client_list_[i].is_leader());
        }
    }
}

uint8_t *ClientManager::GetPayload() {
    return encoded_data_;
}

uint32_t ClientManager::GetPayloadSize() {
    return encoded_data_size_;
}
