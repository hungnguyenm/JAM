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

    memset(&myaddr, 0, sizeof myaddr);
    memset(&myaddr1, 0, sizeof myaddr1);

    std::string myname = "Bipeen";
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(3490);
    inet_aton("194.158.169.137", &myaddr.sin_addr);

    std::string myname1 = "Chris";
    myaddr1.sin_family = AF_INET;
    myaddr1.sin_port = htons(3490);
    inet_aton("193.159.170.137", &myaddr1.sin_addr);

    ClientManager cm = ClientManager();
    printf("Printing clients:\n");

    cm.PrintClients();

    printf("Finished Printing clients:\n");

    cm.AddClient(myaddr, myname, true);
    cm.AddClient(myaddr1, myname1, false);

    std::cout << "Print single cleint " << cm.PrintSingleClientIP(myaddr).c_str() << std::endl;

    printf("Added clients:\n");

    std::cout << "Size: \n " << sizeof myaddr1 << std::endl;

//  cm.GetHigherOrderClients(myaddr);
//  cm.RemoveClient(myaddr1);

    std::vector<ClientInfo> all_clients = cm.GetAllClients();

    printf("All Clients: ");
    for (int i=0; i<all_clients.size(); i++) {
        printf("%s\n", inet_ntoa(all_clients[i].GetSockAddress().sin_addr));
    }


//    std::vector<ClientInfo> higher_order_clients = cm.GetHigherOrderClients(myaddr1);
//    printf("Higher Order Clients: ");
//    for (int i=0; i<higher_order_clients.size(); i++) {
//        printf("%s\n", inet_ntoa(higher_order_clients[i].GetSockAddress().sin_addr));
//    }

    std::cout << "Removed Clients" << std::endl;
    //cm.RemoveAllClients();

    cm.PrintClients();

    JamStatus jm = cm.EncodeClientList();

    std::cout << "Encoded clients:" << std::endl;

    cm.PrintClients();
    cm.DecodeBufferToClientList(cm.GetPayload(), cm.GetPayloadSize());

    cm.PrintClients();

//    jm = cm.DecodeClientList(pld, len);

    std::cout << "All Clients after Decode: \n" << std::endl;

    all_clients = cm.GetAllClients();

    for (int i=0; i<all_clients.size(); i++) {
        printf("%s\n", inet_ntoa(all_clients[i].GetSockAddress().sin_addr));
    }

    return 0;
}