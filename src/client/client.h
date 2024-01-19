// Client.h
#pragma once

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <algorithm>

#include <fstream>
#include <sstream>

using namespace std;

class Client {
private:
    int clientSocket;
    std::thread receivingThread;

    void receiveData();

public:
    Client();
    ~Client();
    
    bool connectToServer(const std::string& ipAddress, int port);
    void displayMenu();
    void startCommunication();
    void closeConnection();
    void handleUserInteraction();

    void sendFile(const std::string& filePath);
    
    void runMenu();
};
