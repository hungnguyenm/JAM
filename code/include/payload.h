/**
 * Payload object sent over the network via UDP wrapper.
 *
 * There are 2 different payloads:
 *  + Normal communication payload
 *  + ACK payload
 *
 * @author: Hung Nguyen
 * @version 1.0 03/31/16
 */

#ifndef JAM_PAYLOAD_H
#define JAM_PAYLOAD_H

#include "config.h"
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

#include <sys/socket.h>
#include <netdb.h>

#define QUIT_MSG_LENGTH 4       // Self-terminate message for UdpReader (must be different than payload length)

enum MessageType : uint8_t {
    CHAT_MSG, STATUS_MSG, ELECTION_MSG, RECOVER_MSG, ACK_MSG, NA
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
    Payload();

    // Init Payload with ACK type and encoded byte stream
    Payload(uint32_t uid, AckStatus ack);

    // Copy constructor
    Payload(const Payload &payload);

    ~Payload();

    const sockaddr_in &GetAddress() const;

    void SetAddress(const sockaddr_in &address);

    EncryptOption GetEncryption() const;

    void SetEncryption(EncryptOption encrypt);

    MessageType GetType() const;

    void SetType(MessageType type);

    uint32_t GetUid() const;

    void SetUid(uint32_t uid);

    AckStatus GetAck() const;

    void SetAck(AckStatus ack);

    int32_t GetOrder() const;

    void SetOrder(int32_t order);

    std::size_t GetLength() const;

    void SetLength(uint32_t length);

    uint32_t GetUsernameLength() const;

    uint32_t GetMessageLength() const;

    std::string GetUsername();

    JamStatus SetUsername(std::string username);

    std::string GetMessage();

    JamStatus SetMessage(std::string message);

    const uint8_t *payload() const;

    uint8_t *payload();

    /**
     * Computes byte stream payload from private variables
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus EncodePayload();


    /**
     * Computes byte stream ack payload without prior private variables
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus EncodeAckPayload(uint32_t uid, AckStatus ack);

    /**
     * Extracts private variables from byte stream payload
     *
     * @return          SUCCESS on normal operation, other JamStatus errors otherwise
     */
    JamStatus DecodePayload();

private:
    enum {
        ACK_LENGTH = 5,             // byte stream length for ACK message
        HEADER_LENGTH = 18          // header byte stream length for normal message
    };

    sockaddr_in address_;           // Sender/Receiver address (IPv4 only)
    EncryptOption encrypt_;
    uint8_t username_[MAX_USER_NAME_LENGTH];
    uint8_t message_[MAX_MESSAGE_LENGTH];

    // The following variables are encoded in header in the same order
    MessageType type_;              // Payload type
    uint32_t uid_;                  // Unique sequence ID for each client

    // -- If ACK payload
    AckStatus ack_;                 // ACK status
    // -- If normal payload
    int32_t order_;                 // -1 for no total-ordering
    union {                         // Payload code for each message type
        Status status;
        ElectionCommand election;
        RecoverCommand recover;
    } code_;
    uint32_t username_length_;      // User name string length in payload
    uint32_t message_length_;       // Message string length in payload

    // Actual payload in byte stream
    uint8_t payload_[HEADER_LENGTH + MAX_USER_NAME_LENGTH + MAX_MESSAGE_LENGTH];
    uint32_t length_;

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
