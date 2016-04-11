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
    inline static JamStatus SendMessage(int fd, const std::string& username, const std::string& message)
    {
        JamStatus status = SendData(fd, username);

        if(status == JamStatus::SUCCESS) {
            status = SendData(fd, message);
        }

        return status;
    }
    inline static JamStatus SendData(int fd, const std::string& data)
    {
        uint32_t dataLength = htonl(data.size());

        ssize_t n = write(fd, &dataLength, sizeof(uint32_t));

        if(n < 0)
        {
            DCERR("Error writing size to socket");
            return JamStatus::ERROR_UNEXPECTED;
        }

        n = write(fd, data.c_str(), data.size());

        if (n < 0)
        {
            DCERR("Error writing data to socket");
            return JamStatus::ERROR_UNEXPECTED;
        }

        return JamStatus::SUCCESS;
    }

    inline static std::string ListenForData(int fd)
    {
        uint32_t dataLength;

        read(fd, &dataLength, sizeof(uint32_t));
        dataLength = ntohl(dataLength);

        std::vector<char> receiveBuffer = std::vector<char>(dataLength);
        ssize_t receivedLength = read(fd, receiveBuffer.data(), dataLength);

        std::string receivedData;
        receivedData.assign(receiveBuffer.data(), receiveBuffer.size());

        if(receivedLength != dataLength) {
            DCERR("Did not receive the expected data");
        }

        return receivedData;
    }

};
#endif //JAM_STREAMCOMMUNICATOR_H
