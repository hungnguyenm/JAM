/**
 * General configuration for entire project.
 *
 * @author: Hung Nguyen
 * @version 1.0 03/31/16
 */

#ifndef JAM_CONFIG_H
#define JAM_CONFIG_H

#define DEFAULT_PORT            "9346"  // Default UDP datagram port
#define MIN_PORT                9000    // Port must be greater than MIN_PORT
#define MAX_MESSAGE_LENGTH      100     // Maximum message length per payload
#define MAX_USER_NAME_LENGTH    20      // Maximum displayed user name length

#define MK_ERROR(x)             (0x00000000|(x))

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

    UDP_SEND_ERROR                      = MK_ERROR(0x4001),
    UDP_DISTRIBUTE_ERROR                = MK_ERROR(0x4002)
};

#endif //JAM_CONFIG_H
