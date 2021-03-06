/**
 * Payload object sent over the network via UDP wrapper.
 *
 * There are 3 different payloads:
 *  + Normal communication payload
 *  + ACK payload
 *  + Self-terminate payload (bound back to terminate threads)
 *
 * @author: Hung Nguyen
 * @version 1.0 03/31/16
 */

#include "../include/payload.h"

using namespace std;

Payload::Payload()
        : type_(NA),
          order_(DEFAULT_NO_ORDER),
          username_length_(0),
          message_length_(0) {
}

Payload::Payload(uint32_t uid, AckStatus ack) {
    EncodeAckPayload(uid, ack);
}

Payload::Payload(const Payload &payload) {
    memcpy(&address_, &(payload.address_), sizeof(address_));
    memcpy(username_, payload.username_, MAX_USER_NAME_LENGTH);
    memcpy(message_, payload.message_, MAX_MESSAGE_LENGTH);

    type_ = payload.type_;
    uid_ = payload.uid_;
    ack_ = payload.ack_;
    order_ = payload.order_;
    code_ = payload.code_;

    username_length_ = payload.username_length_;
    message_length_ = payload.message_length_;
    memcpy(payload_, payload.payload_, HEADER_LENGTH + MAX_USER_NAME_LENGTH + MAX_MESSAGE_LENGTH);
    length_ = payload.length_;
}

void Payload::operator=(const Payload &payload) {
    memcpy(&address_, &(payload.address_), sizeof(address_));
    memcpy(username_, payload.username_, MAX_USER_NAME_LENGTH);
    memcpy(message_, payload.message_, MAX_MESSAGE_LENGTH);

    type_ = payload.type_;
    uid_ = payload.uid_;
    ack_ = payload.ack_;
    order_ = payload.order_;
    code_ = payload.code_;

    username_length_ = payload.username_length_;
    message_length_ = payload.message_length_;
    memcpy(payload_, payload.payload_, HEADER_LENGTH + MAX_USER_NAME_LENGTH + MAX_MESSAGE_LENGTH);
    length_ = payload.length_;
}

bool Payload::operator==(const Payload& other) {
    return (this->GetOrder() == other.GetOrder());
}

bool Payload::operator<(const Payload& other) const {
    return (this->GetOrder() < other.GetOrder());
}

Payload::~Payload() {

}

sockaddr_in *Payload::GetAddress() {
    return &address_;
}

void Payload::SetAddress(const sockaddr_in *address) {
    memcpy(&address_, address, sizeof(address_));
}

MessageType Payload::GetType() const {
    return type_;
}

void Payload::SetType(MessageType type) {
    type_ = type;
    if (type_ == ACK_MSG) {
        length_ = ACK_MSG_LENGTH;
    } else if (type_ != NA) {
        length_ = HEADER_LENGTH + username_length_ + message_length_;
    }
}

uint32_t Payload::GetUid() const {
    return uid_;
}

void Payload::SetUid(uint32_t uid) {
    uid_ = uid;
}

AckStatus Payload::GetAck() const {
    return ack_;
}

void Payload::SetAck(AckStatus ack) {
    Payload::ack_ = ack;
}

int32_t Payload::GetOrder() const {
    return order_;
}

void Payload::SetOrder(int32_t order) {
    order_ = order;
}

Status Payload::GetStatus() const {
    return code_.status;
}

void Payload::SetStatus(Status status) {
    code_.status = status;
}

ElectionCommand Payload::GetElectionCommand() const {
    return code_.election;
}

void Payload::SetElectionCommand(ElectionCommand command) {
    code_.election = command;
}

RecoverCommand Payload::GetRecoverCommand() const {
    return code_.recover;
}

void Payload::SetRecoverCommand(RecoverCommand command) {
    code_.recover = command;
}

size_t Payload::GetLength() const {
    return length_;
}

void Payload::SetLength(uint32_t length) {
    length_ = length;
}


uint32_t Payload::GetUsernameLength() const {
    return username_length_;
}

uint32_t Payload::GetMessageLength() const {
    return message_length_;
}

string Payload::GetUsername() {
    return string(username_, username_ + username_length_);
};

JamStatus Payload::SetUsername(string username) {
    JamStatus ret = SUCCESS;

    if (username.size() < MAX_USER_NAME_LENGTH) {
        username_length_ = (uint32_t) username.size();
        uint32_t i = 0;
        for (i = 0; i < username_length_; ++i) {
            username_[i] = (uint8_t) username[i];
        }
        username_[i] = '\0';
        length_ = HEADER_LENGTH + username_length_ + message_length_;
    } else {
        ret = ERROR_INVALID_PARAMETERS;
    }

    return ret;
};

string Payload::GetMessage() {
    return string(message_, message_ + message_length_);
}

JamStatus Payload::SetMessage(string message) {
    JamStatus ret = SUCCESS;

    if (message.size() < MAX_MESSAGE_LENGTH) {
        message_length_ = (uint32_t) message.size();
        uint32_t i = 0;
        for (i = 0; i < message_length_; ++i) {
            message_[i] = (uint8_t) message[i];
        }
        message_[i] = '\0';
        length_ = HEADER_LENGTH + username_length_ + message_length_;
    } else {
        ret = ERROR_INVALID_PARAMETERS;
    }

    return ret;
};

JamStatus Payload::SetMessage(uint8_t *in, uint32_t length) {
    JamStatus ret = SUCCESS;

    if (length < MAX_MESSAGE_LENGTH) {
        message_length_ = length;
        memcpy(message_, in, length);
        length_ = HEADER_LENGTH + username_length_ + message_length_;
    } else {
        ret = ERROR_INVALID_PARAMETERS;
    }

    return ret;
};

const uint8_t *Payload::payload() const {
    return payload_;
};

uint8_t *Payload::payload() {
    return payload_;
};

JamStatus Payload::EncodePayload() {
    JamStatus ret = SUCCESS;
    uint8_t *buffer = payload_;

    if (ValidateForEncode() == SUCCESS) {
        try {
            packu8(buffer, type_);
            packu32(buffer, uid_);
            packi32(buffer, order_);
            switch (type_) {
                case STATUS_MSG:
                    packu8(buffer, code_.status);
                    break;
                case ELECTION_MSG:
                    packu8(buffer, code_.election);
                    break;
                case RECOVER_MSG:
                    packu8(buffer, code_.recover);
                    break;
                default:
                    // no encode
                    break;
            }
            packu32(buffer, username_length_);
            packu32(buffer, message_length_);
            memcpy(buffer, username_, username_length_);
            buffer += username_length_;
            memcpy(buffer, message_, message_length_);
        } catch (...) {
            ret = ENCODE_ERROR;
        }

    } else {
        ret = ENCODE_VALIDATION_FAILED;
    }

    return ret;
}

JamStatus Payload::EncodeAckPayload(uint32_t uid, AckStatus ack) {
    JamStatus ret = SUCCESS;
    uint8_t *buffer = payload_;

    try {
        // Set private variables accordingly
        type_ = ACK_MSG;
        uid_ = uid;
        ack_ = ack;
        length_ = ACK_MSG_LENGTH;

        // Computes payload
        packu8(buffer, type_);
        packu32(buffer, uid_);
        packu8(buffer, ack_);
    } catch (...) {
        ret = ENCODE_ERROR;
    }

    return ret;
}

JamStatus Payload::EncodeTerminatePayload() {
    JamStatus ret = SUCCESS;

    for (int i = 0; i < QUIT_MSG_LENGTH; ++i) {
        payload_[i] = '0';
    }
    length_ = QUIT_MSG_LENGTH;
    type_ = NA;

    return ret;
}

JamStatus Payload::DecodePayload() {
    JamStatus ret = SUCCESS;
    uint8_t *buffer = payload_;

    if (ValidateForDecode() == SUCCESS) {
        try {
            type_ = (MessageType) unpacku8(buffer);
            uid_ = unpacku32(buffer);
            if (type_ == ACK_MSG) {
                ack_ = (AckStatus) unpacku8(buffer);
            } else {
                order_ = unpacki32(buffer);
                switch (type_) {
                    case STATUS_MSG:
                        code_.status = (Status) unpacku8(buffer);
                        break;
                    case ELECTION_MSG:
                        code_.election = (ElectionCommand) unpacku8(buffer);
                        break;
                    case RECOVER_MSG:
                        code_.recover = (RecoverCommand) unpacku8(buffer);
                        break;
                    default:
                        break;
                }
                if ((username_length_ = unpacku32(buffer)) < MAX_USER_NAME_LENGTH &&
                    ((message_length_ = unpacku32(buffer)) < MAX_MESSAGE_LENGTH)) {
                    memcpy(username_, buffer, username_length_);
                    buffer += username_length_;
                    memcpy(message_, buffer, message_length_);
                } else {
                    ret = DECODE_ERROR;
                }
            }
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

    // Validation logic:
    // + Message type must be set to other than ACK_MSG and NA
    // + If type is Chat Message then it must contain an username length & message length > 0

    switch (type_) {
        case CHAT_MSG:
            if (username_length_ == 0 || message_length_ == 0)
                ret = ENCODE_VALIDATION_FAILED;
            break;
        case ACK_MSG:
            ret = ENCODE_VALIDATION_FAILED;
            break;
        case NA:
            ret = ENCODE_VALIDATION_FAILED;
            break;
        default:
            break;
    }

    return ret;
}

JamStatus Payload::ValidateForDecode() {
    JamStatus ret = SUCCESS;

    // TODO: implement decoding validation algorithm
    return ret;
}

void Payload::clear() {
    type_ = NA;
    uid_ = 0;
    order_ = 0;
    username_length_ = 0;
    message_length_ = 0;
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