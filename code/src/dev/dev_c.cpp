//
// Created by Hung Nguyen on 3/31/16.
// Edited by Krzysztof Jordan from then on :P
//

#include "../../include/user_handler.h"
#include "../../include/stream_communicator.h"

int main(int argc, char* argv[])    {
    CentralQueues queues;

    UserHandler test(&queues);
    std::cout << "Creating a pthread" << std::endl;
    boost::thread t = test.run_on_thread();

    int fd;

//    fd  = test.get_write_pipe();
//    StreamCommunicator::SendMessage(fd, "Bob", "Hello");

    t.join();
    return 0;
}