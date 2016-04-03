/**
 * UDP Wrapper module acts as a low-level I/O handler to send/receive message
 * over UDP datagram reliably.
 *
 * @author: Hung Nguyen
 * @version 1.0 03/31/16
 */

#include "../include/udp_wrapper.h"


UdpWrapper::UdpWrapper()
        : is_ready_(false), uid_(0) {
}

UdpWrapper::~UdpWrapper() {
    if (sockfd_ >= 0)
        close(sockfd_);
}

JamStatus UdpWrapper::Start() {
    JamStatus ret = InitUdpSocket();

    if (ret == SUCCESS) {
        t_reader_ = boost::thread(boost::bind(&UdpWrapper::RunReader, this));
        t_writer_ = boost::thread(boost::bind(&UdpWrapper::RunWriter, this));
    }

    return ret;
}

JamStatus UdpWrapper::Stop() {
    JamStatus ret = SUCCESS;

    if (is_ready_) {
        // TODO: properly handle termination
        t_reader_.join();
        t_writer_.join();
    }

    close(sockfd_);

    return ret;
}

void UdpWrapper::UpdateClientList(std::vector<sockaddr_in> *clients) {
    clients_.clear();
    clients_ = std::vector<sockaddr_in>(*clients);
}

JamStatus UdpWrapper::SendPayload(sockaddr_in addr, Payload payload) {
    JamStatus ret = SUCCESS;

    if (is_ready_) {

    } else {
        ret = UDP_NOT_INIT_ERROR;
    }

    return ret;
}

JamStatus UdpWrapper::DistributePayload(Payload payload) {
    JamStatus ret = SUCCESS;

    if (is_ready_) {

    } else {
        ret = UDP_NOT_INIT_ERROR;
    }

    return ret;
}

JamStatus UdpWrapper::InitUdpSocket() {
    JamStatus ret = SUCCESS;
    addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);    // Make sure struct is empty
    hints.ai_family = AF_INET;          // Only IPv4
    hints.ai_socktype = SOCK_DGRAM;     // UDP stream sockets
    hints.ai_flags = AI_PASSIVE;        // Fill IP automatically

    if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &servinfo) == 0) {
        if ((sockfd_ = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) >= 0) {
            if (bind(sockfd_, servinfo->ai_addr, servinfo->ai_addrlen) == 0) {
                DCOUT("INFO: UdpWrapper - Socket binds successful at port " + std::string(DEFAULT_PORT) + ".");
                freeaddrinfo(servinfo);
                is_ready_ = true;
            } else {
                ret = UDP_BIND_ERROR;
            }
        } else {
            ret = UDP_GET_FD_ERROR;
        }
    } else {
        ret = UDP_GET_ADDR_ERROR;
    }

    return ret;
}

void UdpWrapper::RunReader() {
    DCOUT("INFO: UdpWrapper - Reader started.");

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

void UdpWrapper::RunWriter() {
    DCOUT("INFO: UdpWrapper - Writer started.");

}