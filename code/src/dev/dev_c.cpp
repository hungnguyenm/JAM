//
// Created by Hung Nguyen on 3/31/16.
// Edited by Krzysztof Jordan from then on :P
//

#include "../../include/user_handler.h"
#include "../../include/payload.h"

int main(int argc, char* argv[])    {
    UserHandler test;
    std::cout << "Creating a pthread" << std::endl;
    boost::thread t = test.run_on_thread();

    int fd = test.get_read_pipe();

//    Payload payload;
//    payload.SetType(MessageType::CHAT_MSG);
//    payload.SetUsername("bob");
//    payload.SetMessage("Hi");
//    payload.EncodePayload();
//
//    write(fd, payload.payload(), payload.GetLength());

    t.join();
    return 0;
}