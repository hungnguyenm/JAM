//
// Created by Krzysztof Jordan on 4/10/16.
//

#ifndef JAM_SERIALIZERHELPER_H
#define JAM_SERIALIZERHELPER_H

#include <cstdint>

class SerializerHelper {
public:
    static uint32_t packu8(uint8_t *&buf, uint8_t i);

    static uint32_t packi32(uint8_t *&buf, int32_t i);

    static uint32_t packu32(uint8_t *&buf, uint32_t i);

    static uint8_t unpacku8(uint8_t *&buf);

    static int32_t unpacki32(uint8_t *&buf);

    static uint32_t unpacku32(uint8_t *&buf);

    static uint16_t packu16(uint8_t *&buf, uint16_t i);

    static uint16_t unpacku16(uint8_t *&buf);
};


#endif //JAM_SERIALIZERHELPER_H
