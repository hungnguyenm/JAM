//
// Created by Hung Nguyen on 3/31/16.
//
// C++ includes
#include <vector>
#include <string>
#include <iostream>

void add_client(const std::string& name);
struct Client {
    std::string client_name;
};

std::vector<Client> client_list;

int main() {
    std::cout << "Hello, World!" << std::endl;

    std::string name = "Bipeen";
    add_client(name);
    return 0;
}

void add_client(const std::string& name) {
    Client cl;
    cl.client_name = name;

    client_list.push_back(cl);
}