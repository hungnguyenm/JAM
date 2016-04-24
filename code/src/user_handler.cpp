//
// Created by Krzysztof Jordan on 4/2/16.
//

#include "../include/stream_communicator.h"
#include "../include/user_handler.h"

UserHandler::UserHandler(CentralQueues *queues) :
        queues_(queues) {
    FD_ZERO(&activeFdSet_);
    FD_SET(STDIN_FILENO, &activeFdSet_);

    pipe(incomingFd_);
    FD_SET(incomingFd_[0], &activeFdSet_);
}


UserHandler::~UserHandler() {
    FD_ZERO(&activeFdSet_);

    // Clean up the pipes
    close(incomingFd_[0]);
    close(incomingFd_[1]);
}

void UserHandler::Start() {
    t_run_ = boost::thread(boost::bind(&UserHandler::HandleInput, this));
}

int UserHandler::get_write_pipe() {
    return incomingFd_[1];
}

void UserHandler::WaitOnEnd() {
    t_run_.join();
}

void UserHandler::HandleInput() {
    std::string data;

    while (true) {
        readFdSet_ = activeFdSet_;

        if (select(FD_SETSIZE, &readFdSet_, NULL, NULL, NULL) < 0) {
            DCERR("Select has failed");
            exit(EXIT_FAILURE);
        }

        // Figure out which fd has been set
        if (FD_ISSET(STDIN_FILENO, &readFdSet_)) {

            // grab the input from the command line and write it to the pipe
            getline(std::cin, data);

            if (std::cin.eof() || data == TERMINATE_MESSAGE) {
                DCOUT("INFO: UserHandler - exit captured");
                queues_->signal_terminate();
                break;
            }

            if (data.length() < MAX_MESSAGE_LENGTH) {
                Payload payload;
                payload.SetType(MessageType::CHAT_MSG);
                payload.SetMessage(data);
                queues_->push(CentralQueues::QueueType::USER_OUT, payload);
            } else {
                std::cout << "NOTICE - Message length exceeds maximum. This message is discarded: " <<
                data << std::endl;
            }

        } else if (FD_ISSET(incomingFd_[0], &readFdSet_)) {
            std::string username = StreamCommunicator::ListenForData(incomingFd_[0]);
            std::string message = StreamCommunicator::ListenForData(incomingFd_[0]);

            PrintMessage(username, message);
        }
    }
}

void UserHandler::PrintMessage(const std::string &sender, const std::string &message) {
    std::cout << sender << ":: " << message << std::endl;
}