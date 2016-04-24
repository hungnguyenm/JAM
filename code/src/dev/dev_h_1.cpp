#include "../../include/jam.h"

using namespace std;


int main() {
    const char *user_name = "Hung";
    const char *user_interface = DEFAULT_INTERFACE;
    const char *user_port = DEFAULT_PORT;

    JAM jam;
    jam.StartAsLeader(user_name, user_interface, user_port);

    return 0;
}