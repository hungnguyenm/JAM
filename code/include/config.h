/* ---------------------------------------------------------------------------
 * Just Another Messenger (JAM)
 *
 * config.h
 * General configuration for entire project.
 *
 * @author: Hung Nguyen
 * @version 1.0 03/31/16
 * -------------------------------------------------------------------------*/

#ifndef JAM_CONFIG_H
#define JAM_CONFIG_H

#define MAX_MESSAGE_LENGTH      100     // Maximum message length per payload
#define MAX_USER_NAME_LENGTH    20      // Maximum displayed user name length

#define MK_ERROR(x)             (0x00000000|(x))

enum EncryptOption {
    NO_ENCRYPTION,
    HASH_ONLY,
    ENCRYPT
};

enum JamStatus {
    SUCCESS								= MK_ERROR(0x0000),

    ERROR_UNEXPECTED					= MK_ERROR(0x0001),

    ENCODE_ERROR						= MK_ERROR(0x1001),
    ENCODE_VALIDATION_FAILED            = MK_ERROR(0x1002),

    DECODE_ERROR                        = MK_ERROR(0x2001),
    DECODE_VALIDATION_FAILED            = MK_ERROR(0x2002)
};

#endif //JAM_CONFIG_H
