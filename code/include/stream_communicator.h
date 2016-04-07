//
// Created by Krzysztof Jordan on 4/7/16.
//

#ifndef JAM_STREAMCOMMUNICATOR_H
#define JAM_STREAMCOMMUNICATOR_H

#include <iostream>
#include <netinet/in.h>
#include <string>
#include <vector>
#include <unistd.h>

#include "config.h"

class StreamCommunicator {
public:
    inline static JamStatus SendData(int socketFd, const std::string& data)
    {
        DCOUT(std::string("Sending on ") + std::to_string(socketFd) + " with data: " + data);

        uint32_t dataLength = htonl(data.size());

        ssize_t n = write(socketFd, &dataLength, sizeof(uint32_t));

        if(n < 0)
        {
            DCERR("Error writing size to socket");
            return JamStatus::ERROR_UNEXPECTED;
        }

        n = write(socketFd, data.c_str(), data.size());

        if (n < 0)
        {
            DCERR("Error writing data to socket");
            return JamStatus::ERROR_UNEXPECTED;
        }

        return JamStatus::SUCCESS;
    }

    inline static std::string ListenForData(int socketFd)
    {
        DCOUT(std::string("Listening on ") + std::to_string(socketFd));
        uint32_t dataLength;

        read(socketFd, &dataLength, sizeof(uint32_t));
        dataLength = ntohl(dataLength);

        std::vector<char> receiveBuffer = std::vector<char>(dataLength);
        ssize_t receivedLength = read(socketFd, receiveBuffer.data(), dataLength);

        std::string receivedData;
        receivedData.assign(receiveBuffer.data(), receiveBuffer.size());

        if(receivedLength != dataLength) {
            DCERR("Did not receive the expected data");
        }

        DCOUT(std::string("Received from ") + std::to_string(socketFd) + " and length " + std::to_string(dataLength) + " data: " + receivedData);

        return receivedData;
    }

};
#endif //JAM_STREAMCOMMUNICATOR_H
