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
        terminate_payload.SetUid(uid_++);
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

void UdpWrapper::Join() {
    if (is_ready_) {
        t_reader_.join();
        t_writer_.join();
        t_monitor_.join();
    }
}

void UdpWrapper::UpdateClientAddresses(std::vector<sockaddr_in> *clients) {
    clients_.clear();
    clients_ = std::vector<sockaddr_in>(*clients);
}

JamStatus UdpWrapper::SendPayloadSingle(Payload payload,
                                        const sockaddr_in *addr) {
    JamStatus ret = SUCCESS;

    if (is_ready_) {
        if (ntohs(addr->sin_port) >= MIN_PORT) {
            if (payload.GetLength() > 0) {
                payload.SetUid(uid_++);
                payload.SetAddress(addr);
                out_queue_.push(payload);
            } else {
                ret = UDP_INVALID_PAYLOAD_ERROR;
            }
        } else {
            ret = ERROR_INVALID_PARAMETERS;
        }
    } else {
        ret = UDP_NOT_INIT_ERROR;
    }

    return ret;
}

JamStatus UdpWrapper::SendPayloadSelf(Payload payload) {
    JamStatus ret = SUCCESS;

    if (is_ready_) {
        if (payload.GetLength() > 0) {
            payload.SetUid(uid_++);
            payload.SetAddress(&this_addr_);
            out_queue_.push(payload);
        } else {
            ret = UDP_INVALID_PAYLOAD_ERROR;
        }
    } else {
        ret = UDP_NOT_INIT_ERROR;
    }

    return ret;
}

JamStatus UdpWrapper::SendPayloadList(Payload payload,
                                      std::vector<sockaddr_in> *list) {
    JamStatus ret = SUCCESS;

    if (is_ready_) {
        // TODO: port validation
        if (payload.GetLength() > 0) {
            for (std::vector<sockaddr_in>::iterator it = list->begin();
                 it != list->end(); ++it) {
                payload.SetUid(uid_++);
                payload.SetAddress(&(*it));
                out_queue_.push(payload);
            }
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
        // TODO: port validation
        if (payload.GetLength() > 0) {
            for (std::vector<sockaddr_in>::iterator it = clients_.begin();
                 it != clients_.end(); ++it) {
                payload.SetUid(uid_++);
                payload.SetAddress(&(*it));
                out_queue_.push(payload);
            }
        } else {
            ret = UDP_INVALID_PAYLOAD_ERROR;
        }
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
    int size = 0;

    Payload in_payload;
    Payload ack_payload;

    for (; ;) {
        if ((size = (int) recvfrom(sockfd_, in_payload.payload(), MAX_BUFFER_LENGTH - 1, 0,
                                   (sockaddr *) &clientaddr, &addrlen)) > 0) {
            if (size == QUIT_MSG_LENGTH) {
                DCOUT("INFO: UdpReader - Received terminate message");
                break;
            } else {
                if (in_payload.DecodePayload() == SUCCESS) {
                    if (in_payload.GetType() == ACK_MSG) {
                        DCOUT("INFO: UdpReader - Received ACK message for uid = "
                              + u32_to_string(in_payload.GetUid()));
                        ack_tickets_.erase(in_payload.GetUid());
                    } else {
                        DCOUT("INFO: UdpReader - Received normal payload");
                        ack_payload.EncodeAckPayload(in_payload.GetUid(), ACK_OK);
                        if (sendto(sockfd_, ack_payload.payload(), ack_payload.GetLength(), 0,
                                   (sockaddr *) &clientaddr, addrlen) < 0) {
                            DCERR("ERROR: UdpReader - Failed to send ACK payload");
                        }
                        in_queue_.push(in_payload);
                    }
                } else {
                    DCERR("ERROR: UdpReader - Failed to decode payload");
                }
            }
        } else {
            DCERR("ERROR: UdpReader - Failed to receive packet");
        }
    }
}

void UdpWrapper::RunWriter() {
    Payload payload;

    for (; ;) {
        out_queue_.pop(payload);
        if (payload.GetType() == NA && payload.GetLength() == QUIT_MSG_LENGTH) {
            // Send self-terminate payload and add terminate flag into ack_tickets_
            DCOUT("INFO: UdpWriter - Received terminate message");
            ack_tickets_.insert(payload.GetUid(), NUM_UDP_TERMINATE_RETRIES,
                                std::chrono::duration_cast<std::chrono::milliseconds>
                                        (std::chrono::system_clock::now().time_since_epoch()));
            if (sendto(sockfd_, payload.payload(), payload.GetLength(), 0,
                       (sockaddr *) payload.GetAddress(), sizeof(sockaddr_in)) < 0) {
                DCERR("ERROR: UdpWriter - Failed to send terminate payload");
            }
            break;
        } else {
            DCOUT("INFO: UdpWriter - Sending payload uid = " + u32_to_string(payload.GetUid()));
            if (sendto(sockfd_, payload.payload(), payload.GetLength(), 0,
                       (sockaddr *) payload.GetAddress(), sizeof(sockaddr_in)) >= 0) {
                ack_tickets_.insert(payload.GetUid(), NUM_UDP_RETRIES,
                                    std::chrono::duration_cast<std::chrono::milliseconds>
                                            (std::chrono::system_clock::now().time_since_epoch()));
            } else {
                DCERR("ERROR: UdpWriter - Failed to send payload");

                // TODO: implement send error handler
            }
        }
    }
}

void UdpWrapper::RunMonitor() {
    for (; ;) {
        if (!ack_tickets_.is_empty()) {

        }
    }
}

std::string UdpWrapper::u32_to_string(uint32_t in) {
    std::stringstream ss;
    ss << std::dec << in;
    std::string str;
    ss >> str;

    return str;
}