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
        // Start all task threads
        t_reader_ = boost::thread(boost::bind(&UdpWrapper::RunReader, this));
        t_writer_ = boost::thread(boost::bind(&UdpWrapper::RunWriter, this));
        t_monitor_ = boost::thread(boost::bind(&UdpWrapper::RunMonitor, this));
    }

    return ret;
}

JamStatus UdpWrapper::Stop() {
    JamStatus ret = SUCCESS;

    if (is_ready_) {
        // TODO: properly handle termination
        DCOUT("INFO: UdpWrapper - Initiate termination");

        // Signal to stop reader/writer threads
        Payload terminate_payload;
        terminate_payload.SetAddress(&this_addr_);
        terminate_payload.EncodeTerminatePayload();
        out_queue_.push(terminate_payload);

        t_reader_.join();
        t_writer_.join();
        t_monitor_.join();
    }

    close(sockfd_);

    return ret;
}

void UdpWrapper::UpdateClientAddresses(std::vector<sockaddr_in> *clients) {
    clients_.clear();
    clients_ = std::vector<sockaddr_in>(*clients);
}

JamStatus UdpWrapper::SendPayload(Payload payload) {
    JamStatus ret = SUCCESS;

    if (is_ready_) {
        if (payload.GetLength() > 0) {
            // Assign UID for payload
            payload.SetUid(uid_++);
            out_queue_.push(payload);
        } else {
            ret = UDP_INVALID_PAYLOAD_ERROR;
        }
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

JamStatus UdpWrapper::GetAddressFromInfo(const char *ip,
                                         const char *port,
                                         sockaddr_in *addr) {
    JamStatus ret = SUCCESS;

    if (strtol(port, NULL, 0) > MIN_PORT) {
        addrinfo hints, *servinfo;

        memset(&hints, 0, sizeof hints);    // Make sure struct is empty
        hints.ai_family = AF_INET;          // Only IPv4
        hints.ai_socktype = SOCK_DGRAM;     // UDP stream sockets
        hints.ai_flags = AI_PASSIVE;        // Fill IP automatically

        if (getaddrinfo(ip, port, &hints, &servinfo) == 0) {
            memcpy(addr, (sockaddr_in *) servinfo->ai_addr, servinfo->ai_addrlen);
            freeaddrinfo(servinfo);
        } else {
            DCERR("ERROR: UdpWrapper - Failed to get address");
            ret = UDP_GET_ADDR_ERROR;
        }
    } else {
        ret = ERROR_INVALID_PARAMETERS;
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
            memcpy(&this_addr_, (sockaddr_in *) servinfo->ai_addr, servinfo->ai_addrlen);
            if (bind(sockfd_, servinfo->ai_addr, servinfo->ai_addrlen) == 0) {
                DCOUT("INFO: UdpWrapper - Socket binds successful at port " + std::string(DEFAULT_PORT));
                freeaddrinfo(servinfo);
                is_ready_ = true;
            } else {
                DCERR("ERROR: UdpWrapper - Failed to bind");
                ret = UDP_BIND_ERROR;
            }
        } else {
            DCERR("ERROR: UdpWrapper - Failed to create socket fd");
            ret = UDP_GET_FD_ERROR;
        }
    } else {
        DCERR("ERROR: UdpWrapper - Failed to get address");
        ret = UDP_GET_ADDR_ERROR;
    }

    return ret;
}

void UdpWrapper::RunReader() {
    sockaddr_storage clientaddr;
    socklen_t addrlen = sizeof clientaddr;
    unsigned char buffer[MAX_BUFFER_LENGTH];
    int size = 0;

    for (; ;) {
        if ((size = (int) recvfrom(sockfd_, buffer, MAX_BUFFER_LENGTH - 1, 0,
                                   (sockaddr *) &clientaddr, &addrlen)) > 0) {
            if (size == QUIT_MSG_LENGTH) {
                DCOUT("INFO: UdpReader - Received terminate message");
                break;
            } else {
                // TODO: implement process payload

            }
        } else {
            DCOUT("WARNING: UdpReader - Error receiving packet");
        }
    }
}

void UdpWrapper::RunWriter() {
    Payload payload;

    for (; ;) {
        out_queue_.pop(payload);
        if (payload.GetType() == NA && payload.GetLength() == QUIT_MSG_LENGTH) {
            DCOUT("INFO: UdpWriter - Received terminate message");
            if (sendto(sockfd_, payload.payload(), payload.GetLength(), 0,
                       (sockaddr *) payload.GetAddress(), sizeof(sockaddr_in)) < 0) {
                DCERR("ERROR: UdpWriter - Failed to send terminate payload");
            }
            break;
        } else {
            DCOUT("INFO: UdpWriter - Sending payload uid = " + u32_to_string(payload.GetUid()));
            if (sendto(sockfd_, payload.payload(), payload.GetLength(), 0,
                       (sockaddr *) payload.GetAddress(), sizeof(sockaddr_in)) >= 0) {
                Ticket ticket;
                ticket.uid = payload.GetUid();
                ticket.num_retries = NUM_UDP_RETRIES;
                ticket.time_sent = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch());
                ack_queue_.push(ticket);
            } else {
                DCERR("ERROR: UdpWriter - Failed to send payload");

                // TODO: implement send error handler
            }
        }
    }
}

void UdpWrapper::RunMonitor() {

}

std::string UdpWrapper::u32_to_string(uint32_t in) {
    std::stringstream ss;
    ss << std::dec << in;
    std::string str;
    ss >> str;

    return str;
}