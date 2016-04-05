//
// Author: Bipeen Acharya
// Created: 03/30/16
//
#include <iostream>
#include <vector>

#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;
#include "../include/client_manager.h"
#include "../include/payload.h"

// constructor of ClientManager,
ClientManager::ClientManager()
{
    client_list_
}

ClientManager::~ClientManager()                 // destructor, just an example
{
    delete[] string;
}

vector ClientManager::GetAllClients() {
    if (client_list_.size() > 0) {
        return client_list_;
    }
}

vector ClientManager::GetHigherOrderClients(sockaddr_in client) {
    std::vector<sockaddr_in> higher_client_list_;
    void *client_ip_addr, *temp_ip_addr;
    char ipstr[INET6_ADDRSTRLEN];

    client_ip_addr = &(client->sin_addr);

    // prints the ip address as an int.
    printf("%ul", client->sin_addr);

    if (client_list_.size() > 0) {
        for (int i=0; i<client_list_.size(); i++) {
            // TODO: compare client with all other higher clients based on IP address / PORT
            // convert the IP to a string and print it:
            // inet_ntop(p->ai_family, client_ip_addr, ipstr, sizeof ipstr);
            temp_ip_addr = &(client_list_[i].sin_addr);
            if (*(long *)temp_ip_addr > *(long *)client_ip_addr) {
                higher_client_list_.push_back(client_list_[i]);
            }
        }
    }
    return higher_client_list_;
}

void breakip(char ipstr) {
    /* break the ipstr to individual bytes for comparison */
    const char s[2] = ".";
    char *token;
    char iparray[4];

    /* get the first token */
    token = strtok(ipstr, s);
    iparray[0] = token;

    /* walk through other tokens */
    while( token != NULL )
    {
        printf( " %s\n", token );
        token = strtok(NULL, s);
        iparray.
    }
}

void ClientManager::HandleNotification() {

}

void ClientManager::HandleNewClient() {

}

void ClientManager::HandleCrashClient() {


}

void ClientManager::AddClient() {
    client_list_.push_back(payload_.GetAddress());
}

void ClientManager::RemoveClient() {


}