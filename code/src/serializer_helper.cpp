//
// Created by Krzysztof Jordan on 4/10/16.
//

#include <netinet/in.h>

#include "../include/serializer_helper.h"


uint32_t SerializerHelper::packu8(uint8_t *&buf, uint8_t i) {
    *buf++ = i;
    return 1;
}

uint32_t SerializerHelper::packi32(uint8_t *&buf, int32_t i) {
    uint32_t n = htonl(i);
    *buf++ = n >> 24;
    *buf++ = n >> 16;
    *buf++ = n >> 8;
    *buf++ = n;
    return 4;
}

uint32_t SerializerHelper::packu32(uint8_t *&buf, uint32_t i) {
    uint32_t n = htonl(i);
    *buf++ = n >> 24;
    *buf++ = n >> 16;
    *buf++ = n >> 8;
    *buf++ = n;
    return 4;
}

uint8_t SerializerHelper::unpacku8(uint8_t *&buf) {
    return *buf++;
}

int32_t SerializerHelper::unpacki32(uint8_t *&buf) {
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

uint32_t SerializerHelper::unpacku32(uint8_t *&buf) {
    uint32_t ui = ((uint32_t) buf[0] << 24) |
                  ((uint32_t) buf[1] << 16) |
                  ((uint32_t) buf[2] << 8) |
                  buf[3];
    buf += 4;
    uint32_t un = ntohl(ui);
    return un;
}