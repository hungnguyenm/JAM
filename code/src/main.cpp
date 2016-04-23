#include "../include/jam.h"

using namespace std;

const char *user_name;
const char *server_addr;
const char *server_port;

int main(int argc, char *argv[])
{
    JAM jam;
    const char *user_interface = DEFAULT_INTERFACE;
    const char *user_port = DEFAULT_PORT;

    // Parse arguments
    if (argc == 2) {
        user_name = argv[1];
        jam.StartAsLeader(user_name, user_interface, user_port);
    } else if (argc == 4) {
        user_name = argv[1];
        server_addr = argv[2];
        server_port = argv[3];
        jam.StartAsClient(user_name, user_interface, user_port, server_addr, server_port);
    } else {
        cerr << "Invalid arguments!" << endl;
        exit(1);
    }
}
