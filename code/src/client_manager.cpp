//
// Author: Bipeen Acharya
// Created: 03/30/16
//
#include <iostream>
#include <vector>

#include <arpa/inet.h>
#include <algorithm>
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

//vector<ClientInfo> ClientManager::GetHigherOrderClients(sockaddr_in client) {
//    return GetHigherOrderClients(ClientInfo(client));
//}

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

void ClientManager::AddClient(sockaddr_in client, const std::string& username, bool isLeader) {
    AddClient(ClientInfo(client, username, isLeader));

}

void ClientManager::AddClient(ClientInfo client) {
    client_list_.push_back(client);
    //TODO: sort doesnt work
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
    uint32_t L = ClientInfo::GetPacketSize();
    uint32_t target = L*client_list_.size();

    if (encoded_data_ == nullptr) {
        encoded_data_ = new uint8_t[target];
    } else if (encoded_data_size_ != target) {
        delete(encoded_data_);
        encoded_data_ = new uint8_t[target];
    }
    encoded_data_size_ = target;

    for (int i=0; i<client_list_.size(); i++) {
        ClientInfo::EncodeClientInBuffer(client_list_[i], &encoded_data_[i*L]);
    }
}

JamStatus ClientManager::DecodeBufferToClientList(uint8_t *payload, uint32_t length) {
    if(encoded_data_ != nullptr) {
        delete (encoded_data_);
    }
    encoded_data_ = payload;
    encoded_data_size_ = length;

    JamStatus ret;
    uint32_t username_length_;
    sockaddr_in myaddr;
    std::string username;

    uint32_t L = ClientInfo::GetPacketSize();

    if (length % L != 0) {
        ret = BUFFER_INVALID_LENGTH;
    } else {
        for (int i = 0; i < length; i += L) {
            username_length_ = SerializerHelper::unpacku32(encoded_data_);
            if (username_length_ < MAX_USER_NAME_LENGTH) {
                memcpy(username, encoded_data_, username_length_);
                encoded_data_ += username_length_;
            }

            myaddr.sin_family = AF_INET;
            myaddr.sin_addr.s_addr = SerializerHelper::unpacku32(encoded_data_);
            myaddr.sin_port = SerializerHelper::unpacku16(encoded_data_);

            bool isLeader = SerializerHelper::unpacku8(encoded_data_);
            client_list_.push_back(ClientInfo(myaddr, username, isLeader));
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

uint8_t* ClientManager::GetPayload() {
    return encoded_data_;
}

uint32_t ClientManager::GetPayloadSize() {
    return encoded_data_size_;
}
