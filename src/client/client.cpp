#include "client.h"

Client::Client() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Error: Create the socket" << std::endl;
        return;
    }
}

Client::~Client() {
    close(sock);
    if (receivingThread.joinable()) {
        receivingThread.join();
    }
}

bool Client::connectToServer(const std::string& ipAddress, int port) {
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1) {
        std::cerr << "Error: Cannot connect to Server!" << std::endl;
        return false;
    }

    return true;
}

void Client::startReceiving() {
    receivingThread = std::thread([this]() {
        char buffer[4096];
        while (true) {
            int bytesReceived = recv(sock, buffer, 4096, 0);
            if (bytesReceived <= 0) {
                std::cerr << "Error: Server is down" << std::endl;
                close(sock);
                break;
            }
            std::cout << std::string(buffer, 0, bytesReceived) << std::endl;
        }
    });
}

void Client::sendToServer(const std::string& message) {
    send(sock, message.c_str(), message.size() + 1, 0);
}

std::string Client::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");

    if (first == std::string::npos || last == std::string::npos)
        return "";
    else
        return str.substr(first, last - first + 1);
}
