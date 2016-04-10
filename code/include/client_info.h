//
// Created by acharyab on 4/5/16.
//

#ifndef JAM_CLIENT_INFO_H
#define JAM_CLIENT_INFO_H

#include <netinet/in.h>


class ClientInfo {
public:
    ClientInfo(sockaddr_in client);
    bool operator<(const ClientInfo& other);
    bool operator==(const ClientInfo& other);
    sockaddr_in GetSockAddress();

    bool is_leader();
    void set_leader(bool val);
private:
    sockaddr_in client_;
    int ip_address_[4];
    int port_;

    bool isLeader_;
};

#endif //JAM_CLIENT_INFO_H
