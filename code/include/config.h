/**
 * General configuration for entire project.
 *
 * @author: Hung Nguyen
 * @version 1.0 03/31/16
 */

#ifndef JAM_CONFIG_H
#define JAM_CONFIG_H

#define DEFAULT_INTERFACE           "en0"   // Default UDP interface
#define DEFAULT_PORT                "9346"  // Default UDP datagram port
#define DEFAULT_PORT_DEBUG_1        "9347"  // Default UDP datagram port for debugging
#define DEFAULT_PORT_DEBUG_2        "9348"  // Default UDP datagram port for debugging

#define MIN_PORT                    9000    // Port must be greater than MIN_PORT

#define MAX_MESSAGE_LENGTH          100     // Maximum message length per payload
#define MAX_USER_NAME_LENGTH        20      // Maximum displayed user name length
#define MAX_BUFFER_LENGTH           200     // Maximum UDP socket buffer length
#define DEFAULT_NO_ORDER            -1      // Default value for payload

#define MAX_CLIENT_BUFFER_LENGTH    100     // Maximum client list encoded length

#define UDP_RECEIVER_QUEUE_SIZE     100     // Number of payload receiving kept track by receiver to prevent duplicate
#define NUM_UDP_RETRIES             3       // Default number of UDP resend before notify crash
#define UDP_TIMEOUT                 2000    // Timeout before trying resend UDP payload in miliseconds
#define ACK_MONITOR_INTERVAL        1       // Ack check interval in seconds

#define JAM_CENTRAL_TIMEOUT         1000    // Timeout for main jam waiting internal communication in miliseconds
#define JOIN_TIMEOUT                10000   // Timeout to join chat group in miliseconds

#define TERMINATE_WAIT              2       // Waiting time for each thread to terminate in seconds

#define MK_ERROR(x)                 (0x00000000|(x))

#define PING_INTERVAL               5000

#ifdef DEBUG
#define DCOUT(str) do { std::cout << "DEBUG - " << str << std::endl; } while ( false )
#define DCERR(str) do { \
        std::cerr << "DEBUG - "; \
        if (errno == 0) { \
            std::cerr << str << std::endl; \
        } else { \
            perror(str); \
        } } while ( false )
#else
#define DCOUT(str) do { } while ( false )
#define DCERR(str) do { } while ( false )
#endif

enum EncryptOption {
    NO_ENCRYPTION,
    ENCRYPT
};

enum JamStatus {
    SUCCESS								= MK_ERROR(0x0000),

    ERROR_UNEXPECTED					= MK_ERROR(0x0001),
    ERROR_INVALID_PARAMETERS            = MK_ERROR(0x0002),

    ENCODE_ERROR						= MK_ERROR(0x1001),
    ENCODE_VALIDATION_FAILED            = MK_ERROR(0x1002),

    DECODE_ERROR                        = MK_ERROR(0x2001),
    DECODE_VALIDATION_FAILED            = MK_ERROR(0x2002),

    UDP_GET_ADDR_ERROR                  = MK_ERROR(0x3001),
    UDP_GET_FD_ERROR                    = MK_ERROR(0x3002),
    UDP_BIND_ERROR                      = MK_ERROR(0x3003),
    UDP_NOT_INIT_ERROR                  = MK_ERROR(0x3004),

    UDP_INVALID_PAYLOAD_ERROR           = MK_ERROR(0x4001),
    UDP_SEND_ERROR                      = MK_ERROR(0x4002),
    UDP_DISTRIBUTE_ERROR                = MK_ERROR(0x4003),

    CLIENT_EXCEED_MAXIMUM               = MK_ERROR(0x5001),
    CLIENT_BUFFER_INVALID_LENGTH        = MK_ERROR(0x5002),
};

#endif //JAM_CONFIG_H
