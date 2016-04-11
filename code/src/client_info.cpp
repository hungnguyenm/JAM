//
// Created by acharyab on 4/5/16.
//
#include "../include/client_info.h"

#include <arpa/inet.h>
#include <iosfwd>
#include <cstring>
#include <sstream>

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

    CMP(ntohl(client_.sin_addr.s_addr), ntohl(other.client_.sin_addr.s_addr));
    CMP(ntohs(client_.sin_port), ntohs(other.client_.sin_port));

    return false;
#undef CMP
}

bool ClientInfo::operator==(const ClientInfo &other) {
#define CMP(a, b) return a == b ? true : false;

    CMP(client_.sin_family, other.client_.sin_family);

    CMP(ntohl(client_.sin_addr.s_addr), ntohl(other.client_.sin_addr.s_addr));
    CMP(ntohs(client_.sin_port), ntohs(other.client_.sin_port));

#undef CMP
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

JamStatus ClientInfo::EncodeClientInBuffer(ClientInfo client, uint8_t *buffer) {
    JamStatus ret;

    std::string username = client.get_username();
    uint32_t username_length_ = (uint32_t) username.size() + 1;

    SerializerHelper::packu32(buffer, username_length_);
    memcpy(buffer, username.c_str(), username_length_);
    buffer += username_length_;

    SerializerHelper::packu32(buffer, client.GetSockAddress().sin_addr.s_addr);
    SerializerHelper::packu16(buffer, client.GetSockAddress().sin_port);
    SerializerHelper::packu8(buffer, client.is_leader());
    ret = SUCCESS;

    return ret;
}