/**
 * UDP Reader monitors socket and processes incoming packets
 *
 * @author: Hung Nguyen
 * @version 1.0 04/03/16
 */

#include "../include/config.h"
#include "../include/payload.h"
#include "../include/udp_reader.h"

using namespace std;

UdpReader::UdpReader() {
}

UdpReader::~UdpReader() {

}

void UdpReader::Init(int *sockfd) {
    sockfd_ = *sockfd;
}

void UdpReader::operator()() {
    DCOUT("INFO: UdpReader - Thread started.");

    sockaddr_storage clientaddr;
    socklen_t addrlen = sizeof clientaddr;
    unsigned char buffer[MAX_BUFFER_LENGTH];
    int size = 0;

    for (; ;) {
        if ((size = (int) recvfrom(sockfd_, buffer, MAX_BUFFER_LENGTH - 1, 0,
                                   (sockaddr *) &clientaddr, &addrlen)) > 0) {
            if (size == QUIT_MSG_LENGTH) {
                DCOUT("INFO: UdpReader - Receive terminate message.");
                break;
            } else {
                // TODO: Implement process payload

            }
        } else {
            DCOUT("WARNING: UdpReader - Error receiving packet.");
        }
    }
}