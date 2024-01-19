// Server.h
#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <thread>
#include <map>
#include <fstream>
#include <system_error>
#include <pthread.h>

using namespace std;

class UserAuthentication {
private:
    std::string username;
    std::string password;

public:
    UserAuthentication();
    bool isUserRegistered(const std::string& checkUsername);
    bool registerUser(const std::string& enteredUsername, const std::string& enteredPassword);
    bool isLoggedIn(const std::string& enteredUsername, const std::string& enteredPassword);
};

class TCPServer {
private:
    int listening;
    int clientSocket;
    UserAuthentication auth;

private:
    void handleRegistration(int clientSocket);
    void handleLogin(int clientSocket);
    void processCase();
    void receiveData();
    void sendData(const std::string& data);
    void handleClient(int clientSocket);
    void closeConnection();
    void displayClientInfo(const sockaddr_in& client);
    

public:
    TCPServer();
    bool initServer(int port);
    void start();
};
