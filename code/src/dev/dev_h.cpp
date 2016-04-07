#include "../../include/jam.h"

using namespace std;

int main() {
    const char *username = "Hung";

    JAM jam;
    jam.StartAsLeader(username);

    return 0;
}