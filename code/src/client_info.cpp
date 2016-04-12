//
// Created by acharyab on 4/5/16.
//
#include "../include/client_info.h"

#include <arpa/inet.h>
#include <iosfwd>
#include <cstring>
#include <sstream>

ClientInfo::ClientInfo(sockaddr_in client) :
    client_(client) {

}

ClientInfo::ClientInfo(sockaddr_in client, const std::string &username, bool isLeader) :
        client_(client), username_(username), isLeader_(isLeader) {
    char ipstr[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(client_.sin_addr), ipstr, sizeof ipstr);

    std::stringstream ss(ipstr);
    std::string token;

    for (int i = 0; i < 4; ++i) {
        std::getline(ss, token, '.');
        ip_address_[i] = std::stoi(token);
    }

    port_ = ntohs(client_.sin_port);
}

bool ClientInfo::operator<(const ClientInfo &other) {
#define CMP(a, b) if (a != b) return a < b ? true : false;

    CMP(client_.sin_family, other.client_.sin_family);
    CMP(client_.sin_addr.s_addr, other.client_.sin_addr.s_addr);
    CMP(client_.sin_port, other.client_.sin_port);

    return false;
#undef CMP
}

bool ClientInfo::operator==(const ClientInfo &other) {
        return ((client_.sin_addr.s_addr == other.client_.sin_addr.s_addr) &&
                (client_.sin_port == other.client_.sin_port));
}

sockaddr_in ClientInfo::GetSockAddress() {
    return client_;
}

std::string ClientInfo::get_username() {
    return username_;
}

bool ClientInfo::is_leader() {
    return isLeader_;
}

void ClientInfo::set_leader(bool val) {
    isLeader_ = val;
}

uint32_t ClientInfo::GetPacketSize() {
    //Sum of sizes of username, ip_address, port, boolean(isLeader)
    uint32_t L = (sizeof(uint32_t) + MAX_USER_NAME_LENGTH + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t));
    return L;
}

uint32_t ClientInfo::EncodeClientInBuffer(ClientInfo client, uint8_t *&buffer) {
    uint32_t size = 0;

    std::string username = client.get_username();
    uint32_t username_length_ = (uint32_t) username.size();

    size += SerializerHelper::packu32(buffer, username_length_);
    memcpy(buffer, username.c_str(), username_length_);
    buffer += username_length_;
    size += username_length_;

    size += SerializerHelper::packu32(buffer, client.GetSockAddress().sin_addr.s_addr);
    size += SerializerHelper::packu16(buffer, client.GetSockAddress().sin_port);
    size += SerializerHelper::packu8(buffer, client.is_leader() ? (uint8_t) 1 : (uint8_t) 0);

    return size;
}