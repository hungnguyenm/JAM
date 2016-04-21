/**
 * UDP Wrapper module acts as a low-level I/O handler to send/receive message
 * over UDP datagram reliably.
 *
 * @author: Hung Nguyen
 * @version 1.0 03/31/16
 */

#include "../include/udp_wrapper.h"

using namespace std::chrono;


UdpWrapper::UdpWrapper(CentralQueues *queues)
        : is_ready_(false), uid_(0), queues_(queues) {
}

UdpWrapper::~UdpWrapper() {
    if (sockfd_ >= 0)
        close(sockfd_);
}

JamStatus UdpWrapper::Start(const char *port) {
    JamStatus ret = InitUdpSocket(port);

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

        if (t_reader_.try_join_for(boost::chrono::seconds(TERMINATE_WAIT)) &&
            t_writer_.try_join_for(boost::chrono::seconds(TERMINATE_WAIT)) &&
            t_monitor_.try_join_for(boost::chrono::seconds(TERMINATE_WAIT))) {
            DCOUT("INFO: UdpWrapper - Stopped");
        } else {
            DCOUT("INFO: UdpWrapper - Threads are still running; exit anyway");
        }
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

JamStatus UdpWrapper::SendPayloadSingle(Payload payload,
                                        const sockaddr_in *addr) {
    JamStatus ret = SUCCESS;

    if (is_ready_) {
        LeaderRecover();

        if (ntohs(addr->sin_port) >= MIN_PORT) {
            payload.SetUid(uid_++);
            payload.SetAddress(addr);
            if (payload.EncodePayload() == SUCCESS) {
                out_queue_.push(payload);
            } else {
                ret = ENCODE_VALIDATION_FAILED;
                DCERR("ERROR: UdpWrapper - Encode validation failed");
            }
        } else {
            ret = ERROR_INVALID_PARAMETERS;
            DCERR("ERROR: UdpWrapper - Invalid payload parameters");
        }
    } else {
        ret = UDP_NOT_INIT_ERROR;
    }

    return ret;
}

JamStatus UdpWrapper::SendPayloadSelf(Payload payload) {
    JamStatus ret = SUCCESS;

    if (is_ready_) {
        LeaderRecover();

        payload.SetUid(uid_++);
        payload.SetAddress(&this_addr_);
        if (payload.EncodePayload() == SUCCESS) {
            out_queue_.push(payload);
        } else {
            ret = ENCODE_VALIDATION_FAILED;
            DCERR("ERROR: UdpWrapper - Encode validation failed");
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
        LeaderRecover();

        // TODO: port validation
        for (sockaddr_in addr : *list) {
            payload.SetUid(uid_++);
            payload.SetAddress(&addr);
            if (payload.EncodePayload() == SUCCESS) {
                out_queue_.push(payload);
            } else {
                ret = ENCODE_VALIDATION_FAILED;
                DCERR("ERROR: UdpWrapper - Encode validation failed");
            }
        }
    } else {
        ret = UDP_NOT_INIT_ERROR;
    }

    return ret;
}

JamStatus UdpWrapper::GetAddressFromInfo(const char *addr,
                                         const char *port,
                                         sockaddr_in *sockaddr) {
    JamStatus ret = SUCCESS;

    if (strtol(port, NULL, 0) > MIN_PORT) {
        addrinfo hints, *servinfo;

        memset(&hints, 0, sizeof hints);    // Make sure struct is empty
        hints.ai_family = AF_INET;          // Only IPv4
        hints.ai_socktype = SOCK_DGRAM;     // UDP stream sockets
        hints.ai_flags = AI_PASSIVE;        // Fill IP automatically

        if (getaddrinfo(addr, port, &hints, &servinfo) == 0) {
            memcpy(sockaddr, (sockaddr_in *) servinfo->ai_addr, servinfo->ai_addrlen);
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

JamStatus UdpWrapper::InitUdpSocket(const char *port) {
    JamStatus ret = SUCCESS;
    addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);    // Make sure struct is empty
    hints.ai_family = AF_INET;          // Only IPv4
    hints.ai_socktype = SOCK_DGRAM;     // UDP stream sockets
    hints.ai_flags = AI_PASSIVE;        // Fill IP automatically

    if (getaddrinfo(NULL, port, &hints, &servinfo) == 0) {
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

    // TODO: optimize duplicate monitor algorithm
    std::deque<std::tuple<uint32_t, uint16_t, uint32_t>> receive_queue;

    for (; ;) {
        if ((size = (int) recvfrom(sockfd_, in_payload.payload(), MAX_BUFFER_LENGTH - 1, 0,
                                   (sockaddr *) &clientaddr, &addrlen)) > 0) {
            if (size == QUIT_MSG_LENGTH) {
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
                        if (already_received(&receive_queue,
                                             ((sockaddr_in *) &clientaddr)->sin_addr.s_addr,
                                             ((sockaddr_in *) &clientaddr)->sin_port,
                                             in_payload.GetUid())) {
                            DCOUT("INFO: UdpReader - Duplicate payload occurred");
                        } else {
                            in_payload.SetAddress((sockaddr_in *) &clientaddr);
                            (*queues_).push(CentralQueues::QueueType::UDP_IN, in_payload);
                        }
                    }
                } else {
                    DCERR("ERROR: UdpReader - Failed to decode payload");
                }
            }
        } else {
            DCERR("ERROR: UdpReader - Failed to receive packet");
        }
    }
    DCOUT("INFO: UdpReader - Received terminate message");
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
                                        (std::chrono::system_clock::now().time_since_epoch()),
                                payload);
            if (sendto(sockfd_, payload.payload(), payload.GetLength(), 0,
                       (sockaddr *) payload.GetAddress(), sizeof(sockaddr_in)) < 0) {
                DCERR("ERROR: UdpWriter - Failed to send terminate payload");
            }
            break;
        } else {
            DCOUT("INFO: UdpWriter - Sending payload uid = " + u32_to_string(payload.GetUid()));
            if (sendto(sockfd_, payload.payload(), payload.GetLength(), 0,
                       (sockaddr *) payload.GetAddress(), sizeof(sockaddr_in)) >= 0) {
                ack_tickets_.insert(payload.GetUid(), (uint8_t) NUM_UDP_RETRIES,
                                    duration_cast<milliseconds>(system_clock::now().time_since_epoch()),
                                    payload);
            } else {
                DCERR(std::string("ERROR: UdpWriter - Failed to send payload uid = " +
                                  u32_to_string(payload.GetUid())).c_str());

                // TODO: implement send error handler
            }
        }
    }
}

void UdpWrapper::RunMonitor() {
    for (; ;) {
        if (!ack_tickets_.is_empty()) {
            milliseconds current_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
            milliseconds timeout = milliseconds(UDP_TIMEOUT);
            auto tickets = ack_tickets_.get_all();
            for (auto ticket : tickets) {
                if (std::get<1>(ticket) != NUM_UDP_TERMINATE_RETRIES) {
                    errno = 0;
                    uint32_t uid = std::get<0>(ticket);
                    uint8_t num_retries = std::get<1>(ticket);
                    milliseconds elapsed = current_time - std::get<2>(ticket);
                    Payload payload = std::get<3>(ticket);
                    milliseconds max_time = (NUM_UDP_RETRIES - num_retries + 1) * timeout;
                    if (elapsed >= max_time) {
                        // Handle timeout
                        if (num_retries > 0) {
                            DCERR(std::string("WARNING: UdpMonitor - Retrying for payload uid = " +
                                              u32_to_string(uid)).c_str());
                            out_queue_.push(payload);
                            ack_tickets_.insert_or_assign(uid, --num_retries, std::get<2>(ticket), payload);
                        } else {
                            DCERR(std::string("ERROR: UdpMonitor - Timeout for payload uid = " +
                                              u32_to_string(uid)).c_str());
                            (*queues_).push(CentralQueues::QueueType::UDP_CRASH, *payload.GetAddress());
                            ack_tickets_.erase(uid);
                            // If this is payload that need to send to leader then push to queue for recovering
                            leader_failed_queue_.push(payload);
                        }
                    }
                } else {
                    goto exit;
                }
            }
        }
        boost::this_thread::sleep(boost::posix_time::seconds(ACK_MONITOR_INTERVAL));
    }
    exit:
    DCOUT("INFO: UdpMonitor - Received terminate message");
}

void UdpWrapper::LeaderRecover() {
    // TODO: implement this
}

std::string UdpWrapper::u32_to_string(uint32_t in) {
    std::stringstream ss;
    ss << std::dec << in;
    std::string str;
    ss >> str;

    return str;
}

bool UdpWrapper::already_received(std::deque<std::tuple<in_addr_t, in_port_t, uint32_t>> *queue,
                                  in_addr_t ip,
                                  in_port_t port,
                                  uint32_t uid) {
    for (auto log : *queue) {
        if (std::get<0>(log) == ip && std::get<1>(log) == port && std::get<2>(log) == uid) {
            return true;
        }
    }

    // Entry is not exists in the log
    (*queue).push_front(std::make_tuple(ip, port, uid));
    if ((*queue).size() > UDP_RECEIVER_QUEUE_SIZE)
        (*queue).pop_back();
    return false;
}