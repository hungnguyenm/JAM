//
// Created by acharyab on 3/31/16.
//
#include <iostream>
#include <vector>

using namespace std;
#include "../include/client_manager.h"

// constructor of ClientManager,
ClientManager::ClientManager()
{
    string = new char[10]();
}

ClientManager::~ClientManager()                 // destructor, just an example
{
    delete[] string;
}

vector ClientManager::GetAllClients() {
    return _client_list;
}

vector ClientManager::GetHigherOrderClients() {
    for (int i=0; i<_client_list.size(); i++) {
                
    }
    return _client_list;
}

void ClientManager::HandleNotification() {

}

void ClientManager::HandleNewClient() {

}

void ClientManager::HandleCrashClient() {


}

void ClientManager::AddClient() {

}

void ClientManager::RemoveClient() {


}