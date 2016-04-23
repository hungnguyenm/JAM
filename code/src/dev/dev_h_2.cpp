#include "../../include/jam.h"

using namespace std;

int main(int argc, char** argv) {
    const char *user_name = "Alice";
    const char *user_interface = DEFAULT_INTERFACE;
    const char *user_port = DEFAULT_PORT;
    const char *serv_addr = "127.0.0.1";
    const char *serv_port = DEFAULT_PORT;


    JAM jam;
    if(argc == 2) {
        user_port = argv[1];
    }

    jam.StartAsClient(user_name, user_interface, user_port, serv_addr, serv_port);

    return 0;
}