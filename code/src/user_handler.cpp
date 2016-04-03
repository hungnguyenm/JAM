//
// Created by Krzysztof Jordan on 4/2/16.
//

#include "../include/user_handler.h"
#include "../include/payload.h"

UserHandler::UserHandler() {
    FD_ZERO(&activeFdSet_);
    FD_SET(STDIN_FILENO, &activeFdSet_);

    pipe(fd_);
    FD_SET(fd_[0], &activeFdSet_);
}

UserHandler::~UserHandler() {

}

boost::thread UserHandler::run_on_thread() {
    return boost::thread(*this);
}

int UserHandler::get_read_pipe() {
    return fd_[1];
}

void UserHandler::operator()() {
    while(true) {
        if(select(FD_SETSIZE, &activeFdSet_, NULL, NULL, NULL) < 0) {
            std::cerr << "Select has failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::string data;
        Payload payload;

        // Figure out which fd has been set
        if(FD_ISSET(STDIN_FILENO, &activeFdSet_)) {
            if(std::cin.eof()) {
                // ctrl+d has been input
                break;
            }

            // grab the input from the command line and write it to the pipe
            getline(std::cin, data);

            payload.SetType(MessageType::CHAT_MSG);
            payload.SetMessage(data);

            if(payload.EncodePayload() == JamStatus::SUCCESS) {
                write(fd_[0], payload.payload(), payload.GetLength());
                std::cout << "Sent " << data << std::endl;
            } else {
                std::cerr << "Error encoding local message payload" << std::endl;
            }

        } else if(FD_ISSET(fd_[0], &activeFdSet_)) {
            // grab the input from the pipe  and decode it
            int received = read(fd_[0], payload.payload(), payload.GetLength());
            std::cout << "Got " << received << std::endl;
            payload.DecodePayload();

            if(payload.DecodePayload() != JamStatus::SUCCESS) {
                std::cerr << "Error decoding piped payload" << std::endl;
            } else if(payload.GetType() == MessageType::CHAT_MSG){
                PrintMessage(payload.GetUsername(), payload.GetMessage());
            } else {
                std::cerr << "Unrecognized message " << payload.GetType() << std::endl;
            }
        }
    }

    // Clean up client side of the pipe
    close(fd_[0]);
}

void UserHandler::PrintMessage(const std::string& sender, const std::string& message) {
    std::cout << sender << ":: " << message << std::endl;
}