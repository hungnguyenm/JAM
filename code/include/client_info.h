//
// Created by acharyab on 4/5/16.
//

#ifndef JAM_CLIENT_INFO_H
#define JAM_CLIENT_INFO_H

#include <netinet/in.h>
#include <string>

#include "config.h"
#include "serializer_helper.h"

class ClientInfo {
public:
    ClientInfo(sockaddr_in client);
    ClientInfo(sockaddr_in client, const std::string& username, bool is_Leader = false);

    sockaddr_in get_sock_address();
    static uint32_t get_packet_size();

    static uint32_t EncodeClientInBuffer(ClientInfo client, uint8_t *&buffer);

    std::string get_username();
    bool is_leader();
    void set_leader(bool val);

    void print_client();

    bool operator<(const ClientInfo& other);
    bool operator==(const ClientInfo& other);
private:

    sockaddr_in client_;
    std::string username_;
    bool isLeader_;

    int ip_address_[4];
    int port_;
};

#endif //JAM_CLIENT_INFO_H
