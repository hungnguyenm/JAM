#include "../../include/client_info.h"
#include "../../include/client_manager.h"
#include "../../include/payload.h"

#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <iostream>

int main(){
    struct sockaddr_in myaddr;
    struct sockaddr_in myaddr1;
    struct sockaddr_in myaddr2;

    memset(&myaddr, 0, sizeof myaddr);
    memset(&myaddr1, 0, sizeof myaddr1);
    memset(&myaddr2, 0, sizeof myaddr2);

    std::string myname = "Bipeen";
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(3490);
    inet_aton("194.158.169.137", &myaddr.sin_addr);

    std::string myname1 = "Hung";
    myaddr1.sin_family = AF_INET;
    myaddr1.sin_port = htons(3490);
    inet_aton("193.159.170.137", &myaddr1.sin_addr);

    std::string myname2 = "Chris";
    myaddr2.sin_family = AF_INET;
    myaddr2.sin_port = htons(3490);
    inet_aton("193.159.134.137", &myaddr2.sin_addr);

    ClientManager cm = ClientManager();
    printf("Printing clients:\n");

    cm.PrintClients();

    printf("Finished Printing clients:\n");

    cm.AddClient(myaddr, myname, true);
    cm.AddClient(myaddr1, myname1, false);
    cm.AddClient(myaddr2, myname2, false);


    std::cout << "Print single cleint " << cm.PrintSingleClientIP(myaddr).c_str() << std::endl;

    cm.PrintClients();
    printf("Added clients:\n");

    std::cout << "Size: \n " << sizeof myaddr1 << std::endl;

    //std::string removed_client = cm.RemoveClient(myaddr, myname.c_str());

    //std::cout << "Print after removal of client" << removed_client.c_str() << std::endl;
    cm.PrintClients();

    JamStatus jm = cm.EncodeClientList();

    std::cout << "Encoded clients:" << std::endl;

    cm.PrintClients();

    std::cout << "All Clients after Decode: \n" << std::endl;

    cm.DecodeBufferToClientList(cm.GetPayload(), cm.GetPayloadSize());

    return 0;
}