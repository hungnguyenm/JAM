//
// Created by acharyab on 4/5/16.
//
#include "../include/client_info.h"

#include <arpa/inet.h>
#include <iosfwd>
#include <sstream>

ClientInfo::ClientInfo(sockaddr_in client):
    client_(client)
{
    char ipstr[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(client_.sin_addr), ipstr, sizeof ipstr);

    std::stringstream ss(ipstr);
    std::string token;

    for (int i=0; i<4; ++i) {
        std::getline(ss, token, '.');
        ip_address_[i] = std::stoi(token);
    }

    port_ = ntohs(client_.sin_port);
}

bool ClientInfo::operator<(const ClientInfo& other) {
#define CMP(a, b) if (a != b) return a < b ? true : false;

    CMP(client_.sin_family, other.client_.sin_family);

    CMP(ntohl(client_.sin_addr.s_addr), ntohl(other.client_.sin_addr.s_addr));
    CMP(ntohs(client_.sin_port), ntohs(other.client_.sin_port));

#undef CMP
}

bool ClientInfo::operator==(const ClientInfo& other) {
#define CMP(a, b) return a == b ? true : false;

    CMP(client_.sin_family, other.client_.sin_family);

    CMP(ntohl(client_.sin_addr.s_addr), ntohl(other.client_.sin_addr.s_addr));
    CMP(ntohs(client_.sin_port), ntohs(other.client_.sin_port));

#undef CMP
}



sockaddr_in ClientInfo::GetSockAddress(){
    return client_;
}

bool ClientInfo::is_leader() {
    return isLeader_;
}

void ClientInfo::set_leader(bool val) {
    isLeader_ = val;
}


