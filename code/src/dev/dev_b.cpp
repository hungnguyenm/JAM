#include "../../include/client_info.h"
#include "../../include/client_manager.h"
#include "../../include/payload.h"

#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <iostream>

int main(){
    struct sockaddr_in myaddr;

    int s;

    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(3490);
    myaddr.sin_port = htons(151);
    inet_aton("194.158.169.137", &myaddr.sin_addr);

    struct sockaddr_in myaddr1;

    int s1;

    memset(&myaddr1, 0, sizeof(myaddr1));
    myaddr1.sin_family = AF_INET;
    myaddr1.sin_port = htons(3490);
    myaddr1.sin_port = htons(151);
    inet_aton("193.159.170.137", &myaddr1.sin_addr);

//    ClientInfo cl = ClientInfo(myaddr);
//    ClientInfo cl1 = ClientInfo(myaddr1);
//
//    printf("%d\n", cl < cl1);

    ClientManager cm = ClientManager(myaddr);

    cm.PrintClients();

    cm.AddClient(myaddr1);

    std::cout << "Size: " << sizeof myaddr1 << std::endl;
    cm.PrintClients();

//  cm.GetHigherOrderClients(myaddr);
//  cm.RemoveClient(myaddr1);

    cm.PrintClients();

    std::vector<ClientInfo> all_clients = cm.GetAllClients();

    printf("All Clients: ");
    for (int i=0; i<all_clients.size(); i++) {
        printf("%s\n", inet_ntoa(all_clients[i].GetSockAddress().sin_addr));
    }


    std::vector<ClientInfo> higher_order_clients = cm.GetHigherOrderClients(myaddr1);
    printf("Higher Order Clients: ");
    for (int i=0; i<higher_order_clients.size(); i++) {
        printf("%s\n", inet_ntoa(higher_order_clients[i].GetSockAddress().sin_addr));
    }

    return 0;

}