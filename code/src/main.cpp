#include "../include/jam.h"

using namespace std;

const char *username;
const char *serverip;
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
        serverip = argv[2];
        serverport = argv[3];
        jam.StartAsClient(username, serverip, serverport);
    } else {
        cerr << "Invalid arguments!" << endl;
        exit(1);
    }
}