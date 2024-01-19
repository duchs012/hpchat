#include <iostream>
#include <fstream>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

class FileTransferHandler;

class FileServer {
private:
    int listenfd;
    char fname[100];

public:
    FileServer() : listenfd(0) {
        // Initialize file name
        memset(fname, 0, sizeof(fname));
    }

    ~FileServer() {
        // Close the listening socket
        if (listenfd > 0) {
            close(listenfd);
        }
    }

    bool setupServer(int port) {
        struct sockaddr_in serv_addr;

        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenfd < 0) {
            std::cerr << "Error in socket creation\n";
            return false;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(port);

        if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "Error in bind\n";
            return false;
        }

        if (listen(listenfd, 10) == -1) {
            std::cerr << "Failed to listen\n";
            return false;
        }

        return true;
    }

    void acceptConnections();
    void setFileName(const char* filename);
};

class FileTransferHandler {
private:
    int connfd;
    char fname[100];

public:
    FileTransferHandler(int socket, const char* filename) : connfd(socket) {
        // Initialize file name
        strncpy(fname, filename, sizeof(fname) - 1);
    }

    static void* sendFileToClient(void* arg);
};

void FileServer::acceptConnections() {
    pthread_t tid;
    int connfd;
    struct sockaddr_in c_addr;
    socklen_t clen = sizeof(c_addr);

    while (1) {
        std::cout << "Waiting for connections...\n";

        connfd = accept(listenfd, (struct sockaddr*)&c_addr, &clen);
        if (connfd < 0) {
            std::cerr << "Error in accept\n";
            continue;
        }

        FileTransferHandler* handler = new FileTransferHandler(connfd, fname);
        pthread_create(&tid, NULL, &FileTransferHandler::sendFileToClient, handler);
    }
}

void FileServer::setFileName(const char* filename) {
    strncpy(fname, filename, sizeof(fname) - 1);
}

void* FileTransferHandler::sendFileToClient(void* arg) {
    FileTransferHandler* handler = static_cast<FileTransferHandler*>(arg);

    printf("Connection accepted and id: %d\n", handler->connfd);
    printf("Connected to Client\n");

    // Declare variables here
    FILE* fp = fopen(handler->fname, "rb");
    if (fp == NULL) {
        printf("File open error\n");
        return nullptr;
    }

    unsigned char buff[1024] = {0};
    size_t nread;

    if (write(handler->connfd, handler->fname, strlen(handler->fname) + 1) <= 0) {
        perror("Error sending file name");
        fclose(fp);
        delete handler;
        return nullptr;
    }

    while ((nread = fread(buff, 1, sizeof(buff), fp)) > 0) {
        if (write(handler->connfd, buff, nread) <= 0) {
            perror("Error sending file content");
            break;
        }
    }

    printf("Closing Connection for id: %d\n", handler->connfd);
    close(handler->connfd);
    shutdown(handler->connfd, SHUT_WR);
    sleep(2);

    fclose(fp);
    delete handler;

    return nullptr;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    const char* filename = argv[1];
    FileServer fileServer;
    fileServer.setFileName(filename);

    if (!fileServer.setupServer(5000)) {
        std::cerr << "Failed to set up the server\n";
        return 1;
    }

    fileServer.acceptConnections();

    return 0;
}
