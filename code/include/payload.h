/* ---------------------------------------------------------------------------
 * Just Another Messenger (JAM)
 *
 * payload.h
 * Payload object sent over the network via UDP wrapper.
 *
 * @author: Hung Nguyen
 * @version 1.0 04/01/16
 * -------------------------------------------------------------------------*/

#ifndef JAM_PAYLOAD_H
#define JAM_PAYLOAD_H

#include "config.h"
#include <cstdint>
#include <cstdlib>
#include <string>

enum MessageType : uint8_t {
    CHAT_MSG, STATUS_MSG, ELECTION_MSG, RECOVER_MSG, ACK_MSG
};

enum Status : uint8_t {
    CLIENT_JOIN, CLIENT_LEAVE, CLIENT_CRASH, LEADER_LEAVE
};

enum ElectionCommand : uint8_t {
    ELECT_START, ELECT_STOP, ELECT_WIN
};

enum RecoverCommand : uint8_t {
    MSG_LOST
};

enum AckStatus : uint8_t {
    ACK_OK, ACK_ERROR
};

class Payload {
public:
    Payload(std::string ip, uint32_t port);

    ~Payload();

    const uint8_t *ip() const;

    uint8_t *ip();

    uint32_t GetPort() const;

    void SetPort(uint32_t port);

    EncryptOption GetEncryption() const;

    void SetEncryption(EncryptOption encrypt);

    MessageType GetType() const;

    void SetType(MessageType type);

    int32_t GetOrder() const;

    void SetOrder(int32_t order);

    uint32_t GetUid() const;

    void SetUid(uint32_t uid);

    std::size_t GetLength() const;

    uint32_t GetUsernameLength() const;

    uint32_t GetMessageLength() const;

    const uint8_t *message() const;

    uint8_t *message();

    const uint8_t *username() const;

    uint8_t *username();

    const uint8_t *payload() const;

    uint8_t *payload();

    /*
     * Function:  EncodePayload
     * --------------------
     * computes byte stream payload from private variables
     *
     *  returns: SUCCESS on normal operation
     *           other JamStatus errors otherwise
     */
    JamStatus EncodePayload();

    /*
     * Function:  DecodePayload
     * --------------------
     * extracts private variables from byte stream payload
     *
     *  returns: SUCCESS on normal operation
     *           other JamStatus errors otherwise
     */
    JamStatus DecodePayload();

private:
    enum {
        HEADER_LENGTH = 10          // (in bytes)
    };
    enum {
        IP_LENGTH = 16              // IPv4 only (in bytes)
    };

    uint8_t _ip[IP_LENGTH];
    uint32_t _port;                 // UDP port
    uint32_t _length;               // Total payload length in byte stream
    EncryptOption _encrypt;
    uint8_t _username[MAX_USER_NAME_LENGTH];
    uint8_t _message[MAX_MESSAGE_LENGTH];

    // The following variables are encoded in header in the same order
    MessageType _type;              // Payload type
    int32_t _order;                 // -1 for no total-ordering
    uint32_t _uid;                  // Unique sequence ID for each client
    union {                   // Payload _code for each message type
        Status status;
        ElectionCommand election;
        RecoverCommand recover;
        AckStatus ack;
    } _code;
    uint32_t _username_length;      // User name string length in payload
    uint32_t _message_length;       // Message string length in payload

    // Actual payload in byte stream
    uint8_t _payload[HEADER_LENGTH + MAX_USER_NAME_LENGTH + MAX_MESSAGE_LENGTH];

    // Helper functions
    // -- Validate functions
    JamStatus ValidateForEncode();

    JamStatus ValidateForDecode();

    // -- Serialize/Deserialize functions
    uint32_t packu8(uint8_t *&buf, uint8_t i);

    uint32_t packi32(uint8_t *&buf, int32_t i);

    uint32_t packu32(uint8_t *&buf, uint32_t i);

    uint8_t unpacku8(uint8_t *&buf);

    int32_t unpacki32(uint8_t *&buf);

    uint32_t unpacku32(uint8_t *&buf);

    // -- Cryptography functions
};

#endif //JAM_PAYLOAD_H
