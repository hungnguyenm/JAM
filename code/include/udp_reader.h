/**
 * UDP Reader monitors socket and processes incoming packets
 *
 * @author: Hung Nguyen
 * @version 1.0 04/03/16
 */

#ifndef JAM_UDP_READER_H
#define JAM_UDP_READER_H

#include <iostream>

class UdpReader {
public:
    UdpReader();

    ~UdpReader();

    void SetFd(int *sockfd);

    void operator()();

private:
    int sockfd_;
};

#endif //JAM_UDP_READER_H
