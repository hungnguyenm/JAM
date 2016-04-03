/**
 * UDP Reader monitors socket and processes incoming packets
 *
 * @author: Hung Nguyen
 * @version 1.0 04/03/16
 */

#include "../include/udp_reader.h"

using namespace std;

UdpReader::UdpReader() {
}

UdpReader::~UdpReader() {

}

void UdpReader::SetFd(int *sockfd) {
    sockfd_ = *sockfd;
}

void UdpReader::operator()() {
    cout << "Reader started!" << endl;
}