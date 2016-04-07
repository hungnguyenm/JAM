#include "../../include/client_manager.h"
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>

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

    ClientInfo cl = ClientInfo(myaddr);
    ClientInfo cl1 = ClientInfo(myaddr1);

    printf("%d\n", cl < cl1);

    ClientManager cm = ClientManager(cl);

    cm.AddClient(cl1);

    cm.PrintClients();


    return 0;

}