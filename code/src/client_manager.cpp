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

void ClientManager::set_self_address(sockaddr_in addr) {
    self_addr_ = addr;
}

sockaddr_in ClientManager::get_self_address() {
    return self_addr_;
}

vector<ClientInfo> ClientManager::GetAllClients() {
    return client_list_;
}

std::vector<sockaddr_in> ClientManager::GetAllClientSockAddressWithoutMe() {
    std::vector<sockaddr_in> vectorOfSockAddress;
    if (client_list_.size() > 0) {
        for (int i = 0; i < client_list_.size(); i++) {
            if (client_list_[i].GetSockAddress().sin_addr.s_addr != self_addr_.sin_addr.s_addr ||
                client_list_[i].GetSockAddress().sin_port != self_addr_.sin_port) {
                vectorOfSockAddress.push_back(client_list_[i].GetSockAddress());
            }
        }
    }
    return vectorOfSockAddress;
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

vector<ClientInfo> ClientManager::GetHigherOrderClients(sockaddr_in client) {
    return GetHigherOrderClients(ClientInfo(self_addr_));
}
vector<ClientInfo> ClientManager::GetHigherOrderClients() {
    return GetHigherOrderClients(self_addr_);
}

bool ClientManager::AddClient(sockaddr_in client, const std::string &username, bool isLeader) {
    bool ret = AddClient(ClientInfo(client, username, isLeader));
    return ret;
}

bool ClientManager::AddClient(ClientInfo client) {
    for (int i = 0; i < client_list_.size(); i++) {
        if (client_list_[i] == client) {
            return false;
        }
    }
    client_list_.push_back(client);
    // TODO: implement sort

    EncodeClientList();
    return true;
}

bool ClientManager::RemoveClient(sockaddr_in client,
                                 std::string *username) {
    return RemoveClient(ClientInfo(client), username);
}

bool ClientManager::RemoveClient(ClientInfo client,
                                 std::string *username) {
    bool ret = false;

    for (int i = 0; i < client_list_.size(); i++) {
        if (client_list_[i] == client) {
            *username = client_list_[i].get_username();
            client_list_.erase(client_list_.begin() + i);
            ret = true;
            EncodeClientList();
            break;
        }
    }

    return ret;
}

std::string ClientManager::PrintSingleClientIP(sockaddr_in client) {
    std::string client_ip_information;
    int port;
    char ipstr[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(client.sin_addr), ipstr, sizeof ipstr);
    port = ntohs(client.sin_port);
    client_ip_information += ipstr;
    client_ip_information += ":";
    client_ip_information += std::to_string(port);
    return client_ip_information;
}

void ClientManager::RemoveAllClients() {
    client_list_.clear();
    encoded_data_size_ = 0;
}

JamStatus ClientManager::EncodeClientList() {
    JamStatus ret = SUCCESS;

    uint8_t *buffer = encoded_data_;
    encoded_data_size_ = 0;

    for (int i = 0; i < client_list_.size(); i++) {
        encoded_data_size_ += ClientInfo::EncodeClientInBuffer(client_list_[i], buffer);
    }

    if (encoded_data_size_ > MAX_CLIENT_BUFFER_LENGTH)
        ret = CLIENT_BUFFER_INVALID_LENGTH;

    return ret;
}

JamStatus ClientManager::DecodeBufferToClientList(uint8_t *payload, uint32_t length) {
    JamStatus ret = SUCCESS;

    RemoveAllClients();
    uint8_t *buffer = payload;
    encoded_data_size_ = length;

    uint32_t username_length;
    sockaddr_in addr;
    char username[MAX_USER_NAME_LENGTH];

    if (length > MAX_CLIENT_BUFFER_LENGTH) {
        ret = CLIENT_BUFFER_INVALID_LENGTH;
    } else {
        uint32_t count = 0;
        while (count < encoded_data_size_) {
            username_length = SerializerHelper::unpacku32(buffer);
            if (username_length < MAX_USER_NAME_LENGTH) {
                memcpy(username, buffer, username_length);
                username[username_length] = '\0';
                buffer += username_length;
            }

            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = SerializerHelper::unpacku32(buffer);
            addr.sin_port = SerializerHelper::unpacku16(buffer);

            bool isLeader = SerializerHelper::unpacku8(buffer);
            client_list_.push_back(ClientInfo(addr, username, isLeader));
            count += 4 + username_length + 4 + 2 + 1;
        }
    }

    return ret;
}

void ClientManager::PrintClients() {
    if (client_list_.size() > 0) {
        for (int i = 0; i < client_list_.size(); i++) {
            printf("%s ", client_list_[i].get_username().c_str());
            printf("%s:%d", inet_ntoa(client_list_[i].GetSockAddress().sin_addr),
                   ntohs(client_list_[i].GetSockAddress().sin_port));
            if (client_list_[i].is_leader()) {
                printf(" (Leader)\n");
            } else {
                printf("\n");
            }
        }
    }
}

uint8_t *ClientManager::GetPayload() {
    return encoded_data_;
}

uint32_t ClientManager::GetPayloadSize() {
    return encoded_data_size_;
}

uint32_t ClientManager::EncodeSingleAddress(uint8_t *payload, sockaddr_in *addr) {
    uint32_t size = 0;
    uint8_t *buffer = payload;

    size += SerializerHelper::packu32(buffer, addr->sin_addr.s_addr);
    size += SerializerHelper::packu16(buffer, addr->sin_port);

    return size;
}

JamStatus ClientManager::DecodeSingleAddress(uint8_t *payload, uint32_t length, sockaddr_in *addr) {
    JamStatus ret = SUCCESS;

    if (length == 6) {
        uint8_t *buffer = payload;
        addr->sin_family = AF_INET;
        addr->sin_addr.s_addr = SerializerHelper::unpacku32(buffer);
        addr->sin_port = SerializerHelper::unpacku16(buffer);
    } else {
        ret = CLIENT_BUFFER_INVALID_LENGTH;
    }

    return ret;
}