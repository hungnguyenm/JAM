//
// Created by Krzysztof Jordan on 4/2/16.
//

#include "../include/stream_communicator.h"
#include "../include/user_handler.h"

UserHandler::UserHandler(CentralQueues *queues) :
    queues_(queues)
{
    FD_ZERO(&activeFdSet_);
    FD_SET(STDIN_FILENO, &activeFdSet_);

    pipe(incomingFd_);
    FD_SET(incomingFd_[0], &activeFdSet_);
}

boost::thread UserHandler::run_on_thread() {
    return boost::thread(*this);
}

int UserHandler::get_write_pipe() {
    return incomingFd_[1];
}

void UserHandler::operator()() {
    std::string data;

    while (true) {
        readFdSet_ = activeFdSet_;

        if (select(FD_SETSIZE, &readFdSet_, NULL, NULL, NULL) < 0) {
            DCERR("Select has failed");
            exit(EXIT_FAILURE);
        }

        // Figure out which fd has been set
        if (FD_ISSET(STDIN_FILENO, &readFdSet_)) {
            if (std::cin.eof()) {
                // ctrl+d has been input
                queues_->signal_terminate();
                break;
            }

            // grab the input from the command line and write it to the pipe
            getline(std::cin, data);

            Payload payload;
            payload.SetType(MessageType::LOCAL_MSG);
            payload.SetMessage(data);
            queues_->push(CentralQueues::QueueType::USER_OUT, payload);

        } else if (FD_ISSET(incomingFd_[0], &readFdSet_)) {
            std::string username = StreamCommunicator::ListenForData(incomingFd_[0]);
            std::string message = StreamCommunicator::ListenForData(incomingFd_[0]);

            PrintMessage(username, message);
        }
    }

    // Clean up the pipes
    close(incomingFd_[0]);
    close(incomingFd_[1]);
}

void UserHandler::PrintMessage(const std::string &sender, const std::string &message) {
    std::cout << sender << ":: " << message << std::endl;
}