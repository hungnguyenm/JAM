//
// Author: Bipeen Acharya
// Created: 03/30/16
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
    return client_list_;
}

vector ClientManager::GetHigherOrderClients() {
    for (int i=0; i<_client_list.size(); i++) {
                
    }
    return client_list_;
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