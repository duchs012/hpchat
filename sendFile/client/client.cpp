#include <iostream>
#include <fstream>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class FileReceiver {
private:
    int sockfd;
    struct sockaddr_in serv_addr;
    char recvBuff[1024];

public:
    FileReceiver() {
        sockfd = 0;
        memset(recvBuff, '0', sizeof(recvBuff));

        // Initialize sockaddr_in data structure
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(5000); // port
        serv_addr.sin_addr.s_addr = INADDR_ANY;
    }

    ~FileReceiver() {
        // Close the socket if it is open
        if (sockfd > 0) {
            close(sockfd);
        }
    }

    bool createSocket() {
        // Create a socket
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            std::cerr << "Error: Could not create socket" << std::endl;
            return false;
        }
        return true;
    }

    bool connectToServer(const char* ipAddress) {
        // Attempt a connection
        if (inet_pton(AF_INET, ipAddress, &serv_addr.sin_addr) <= 0) {
            std::cerr << "Invalid address/ Address not supported" << std::endl;
            return false;
        }

        if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "Error: Connect Failed" << std::endl;
            return false;
        }

        std::cout << "Connected to ip: " << inet_ntoa(serv_addr.sin_addr) << " : " << ntohs(serv_addr.sin_port) << std::endl;
        return true;
    }

    void receiveFile() {
        // Receive file name
        char fname[256];  // Adjust the size to match the expected length
        read(sockfd, fname, sizeof(fname));
        std::cout << "File Name: " << fname << std::endl;
        std::cout << "Receiving file..." << std::endl;


        // Create file where data will be stored
        std::ofstream fp(fname, std::ios::binary | std::ios::app);
        if (!fp.is_open()) {
            std::cerr << "Error opening file" << std::endl;
            return;
        }

        long double sz = 1;
        int bytesReceived = 0;

        // Receive data in chunks of 256 bytes
        while ((bytesReceived = read(sockfd, recvBuff, 1024)) > 0) {
            sz++;
            std::cout << "Received: " << (sz / 1024) << " Mb" << std::endl;
            fflush(stdout);
            fp.write(recvBuff, bytesReceived);
        }

        if (bytesReceived < 0) {
            std::cerr << "Read Error" << std::endl;
        }

        std::cout << "File OK....Completed" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Enter IP address to connect: ";
        return 1;
    }

    const char* ipAddress = argv[1];

    FileReceiver fileReceiver;

    if (!fileReceiver.createSocket() || !fileReceiver.connectToServer(ipAddress)) {
        return 1;
    }

    fileReceiver.receiveFile();

    return 0;
}
