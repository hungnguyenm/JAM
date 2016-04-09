/**
 * JAM class - Module Coordinator acts as the link between all internal modules.
 * It functions as the single point of contact of JAM.
 *
 * @author: Hung Nguyen
 * @version 1.0 04/07/16
 */

#include "../include/jam.h"

#include <ifaddrs.h>
#include <arpa/inet.h>

using namespace std;

JAM::JAM()
        : udpWrapper_(&queues_),
          userHandler_(&queues_) {
}

JAM::~JAM() {

}

void JAM::StartAsLeader(const char *username) {
    // INFO
    cout << username << " is starting a new chat group!" << endl;

    // Start UDP Wrapper
    if (udpWrapper_.Start(DEFAULT_PORT) == SUCCESS) {
        cout << "Succeeded, listening on" << GetInterfaceAddress(DEFAULT_PORT) << ". Current users:" << endl;
    } else {
        cerr << "Failed to start new chat group!" << endl;
        exit(1);
    }

    Main();
}

void JAM::StartAsClient(const char *username,
                        const char *addr,
                        const char *port) {
    // INFO
    cout << username << " is joining a chat group at " << addr << ":" << port << "!" << endl;

    // Start UDP Wrapper

    Main();
}

void JAM::Main() {
    // Infinite loop to monitor central communication
    for (; ;) {
        if (queues_.wait_for_data(JAM_CENTRAL_TIMEOUT)) {
            if (queues_.is_terminate())
                break;          // Only exit loop if receive terminate signal

            // Go through each queue and handle data if available
            bool has_data = false;
            do {

            } while (has_data);
        }
    }

    DCOUT("INFO: JAM - Received terminate signal");
    udpWrapper_.Stop();

    std::cout << "Bye." << std::endl;
}

string JAM::GetInterfaceAddress(const char *port) {
    stringstream ss;
    ifaddrs *ifap, *ifa;
    sockaddr_in *sa;                // Only print IPv4
    char *name, *addr;
    const char *sep = "";

    getifaddrs(&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family == AF_INET) {
            name = ifa->ifa_name;
            if (strcmp(name, "lo0") != 0) {     // Ignore loop-back interface
                sa = (sockaddr_in *) ifa->ifa_addr;
                addr = inet_ntoa(sa->sin_addr);
                ss << sep << " (" << (const char *) name << ")" << (const char *) addr << ":" << port;
                sep = ",";
            }
        }
    }

    return ss.str();
}

