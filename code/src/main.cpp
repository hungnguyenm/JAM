#include "../include/jam.h"

using namespace std;

const char *username;
const char *serveraddr;
const char *serverport;

int main(int argc, char *argv[])
{
    JAM jam;

    // Parse arguments
    if (argc == 2) {
        username = argv[1];
        jam.StartAsLeader(username);
    } else if (argc == 4) {
        username = argv[1];
        serveraddr = argv[2];
        serverport = argv[3];
        jam.StartAsClient(username, serveraddr, serverport);
    } else {
        cerr << "Invalid arguments!" << endl;
        exit(1);
    }
}
