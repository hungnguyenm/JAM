/* ---------------------------------------------------------------------------
 * Just Another Messenger (JAM)
 *
 * payload.cpp
 * Payload object sent over the network via UDP wrapper.
 *
 * @author: Hung Nguyen
 * @version 1.0 04/01/16
 * -------------------------------------------------------------------------*/

#include "../include/payload.h"

Payload::Payload(std::string ip, uint32_t port) {

}

Payload::~Payload() {

}

const uint8_t *Payload::ip() const {
    return _ip;
}

uint8_t *Payload::ip() {
    return _ip;
}

uint32_t Payload::GetPort() const {
    return _port;
}

void Payload::SetPort(uint32_t port) {
    _port = port;
}

EncryptOption Payload::GetEncryption() const {
    return _encrypt;
}

void Payload::SetEncryption(EncryptOption encrypt) {
    _encrypt = encrypt;
}

MessageType Payload::GetType() const {
    return _type;
}

void Payload::SetType(MessageType type) {
    _type = type;
}

int32_t Payload::GetOrder() const {
    return _order;
}

void Payload::SetOrder(int32_t order) {
    _order = order;
}

uint32_t Payload::GetUid() const {
    return _uid;
}

void Payload::SetUid(uint32_t uid) {
    _uid = uid;
}

size_t Payload::GetLength() const {
    return HEADER_LENGTH + _username_length + _message_length;
}

uint32_t Payload::GetUsernameLength() const {
    return _username_length;
}

uint32_t Payload::GetMessageLength() const {
    return _message_length;
}

const uint8_t *Payload::message() const {
    return _message;
}

uint8_t *Payload::message() {
    return _message;
}

const uint8_t *Payload::username() const {
    return _username;
}

uint8_t *Payload::username() {
    return _username;
}

const uint8_t *Payload::payload() const {
    return _payload;
};

uint8_t *Payload::payload() {
    return _payload;
};

JamStatus Payload::EncodePayload() {
    JamStatus ret = SUCCESS;
    uint8_t *buffer = _payload;

    if (ValidateForEncode() == SUCCESS) {
        try {
            packu8(buffer, _type);
            packi32(buffer, _order);
            packu32(buffer, _uid);
            switch (_type) {
                case STATUS_MSG:
                    packu8(buffer, _code.status);
                    break;
                case ELECTION_MSG:
                    packu8(buffer, _code.election);
                    break;
                case RECOVER_MSG:
                    packu8(buffer, _code.recover);
                    break;
                case ACK_MSG:
                    packu8(buffer, _code.ack);
                    break;
                default:
                    packu8(buffer, 0);
                    break;
            }
            packu32(buffer, _username_length);
            packu32(buffer, _message_length);
            memcpy(buffer, _username, _username_length);
            buffer += _username_length;
            memcpy(buffer, _message, _message_length);
        } catch (...) {
            ret = ENCODE_ERROR;
        }

    } else {
        ret = ENCODE_VALIDATION_FAILED;
    }

    return ret;
}

JamStatus Payload::DecodePayload() {
    JamStatus ret = SUCCESS;
    uint8_t *buffer = _payload;

    if (ValidateForDecode() == SUCCESS) {
        try {
            _type = (MessageType) unpacku8(buffer);
            _order = unpacki32(buffer);
            _uid = unpacku32(buffer);
            switch (_type) {
                case STATUS_MSG:
                    _code.status = (Status) unpacku8(buffer);
                    break;
                case ELECTION_MSG:
                    _code.election = (ElectionCommand) unpacku8(buffer);
                    break;
                case RECOVER_MSG:
                    _code.recover = (RecoverCommand) unpacku8(buffer);
                    break;
                case ACK_MSG:
                    _code.ack = (AckStatus) unpacku8(buffer);
                    break;
                default:
                    break;
            }
            _username_length = unpacku32(buffer);
            _message_length = unpacku32(buffer);
            memcpy(_username, buffer, _username_length);
            buffer += _username_length;
            memcpy(_message, buffer, _message_length);
        } catch (...) {
            ret = DECODE_ERROR;
        }
    } else {
        ret = DECODE_VALIDATION_FAILED;
    }

    return ret;
}

JamStatus Payload::ValidateForEncode() {
    JamStatus ret = SUCCESS;

    // TODO: implement encoding validation algorithm
    return ret;
}

JamStatus Payload::ValidateForDecode() {
    JamStatus ret = SUCCESS;

    // TODO: implement decoding validation algorithm
    return ret;
}

uint32_t Payload::packu8(uint8_t *&buf, uint8_t i) {
    *buf++ = i;
    return 1;
}

uint32_t Payload::packi32(uint8_t *&buf, int32_t i) {
    uint32_t n = htonl(i);
    *buf++ = n >> 24;
    *buf++ = n >> 16;
    *buf++ = n >> 8;
    *buf++ = n;
    return 4;
}

uint32_t Payload::packu32(uint8_t *&buf, uint32_t i) {
    uint32_t n = htonl(i);
    *buf++ = n >> 24;
    *buf++ = n >> 16;
    *buf++ = n >> 8;
    *buf++ = n;
    return 4;
}

uint8_t Payload::unpacku8(uint8_t *&buf) {
    return *buf++;
}

int32_t Payload::unpacki32(uint8_t *&buf) {
    uint32_t ui = ((uint32_t) buf[0] << 24) |
                  ((uint32_t) buf[1] << 16) |
                  ((uint32_t) buf[2] << 8) |
                  buf[3];
    uint32_t un = ntohl(ui);

    // change unsigned numbers to signed
    int32_t i;
    if (un <= 0x7fffffffu) { i = un; }
    else { i = -1 - (int32_t) (0xffffffffu - un); }
    buf += 4;
    return i;
}

uint32_t Payload::unpacku32(uint8_t *&buf) {
    uint32_t ui = ((uint32_t) buf[0] << 24) |
                  ((uint32_t) buf[1] << 16) |
                  ((uint32_t) buf[2] << 8) |
                  buf[3];
    buf += 4;
    uint32_t un = ntohl(ui);
    return un;
}