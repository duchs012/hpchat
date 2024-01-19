#include "server.h"

UserAuthentication::UserAuthentication() : username(""), password("") {}

bool UserAuthentication::isUserRegistered(const std::string& checkUsername) {
    // Implementation here
    ifstream inputFile("data/users.txt");
    string username;
    while (getline(inputFile, username)) {
        // Check if the current line is the username
        if (username == checkUsername) {
            inputFile.close();
            return true; // User is already registered
        }

        // Skip the next line (password)
        if (!getline(inputFile, username)) {
            break;  // Break if unable to read the next line (no password found)
        }
    }

    inputFile.close();
    return false; // User is not registered
}

bool UserAuthentication::registerUser(const std::string& enteredUsername, const std::string& enteredPassword) {
    // Implementation here
    username = enteredUsername;
    password = enteredPassword;

    // ofstream myFile("data\\" + username + ".txt");
    ofstream myFile("data\\users.txt", ios::app);  // Open the file to continue writing to the last file
    if (myFile.is_open()) {
        myFile << username << endl << password << endl;
        myFile.close();
        return true;
    } else {
        cerr << "Unable to open file" << endl;
        return false;
    }
}

bool UserAuthentication::isLoggedIn(const std::string& enteredUsername, const std::string& enteredPassword) {
    string storedUsername, storedPassword;

    ifstream inputFile("./data/data_users/users.txt");
    if (inputFile.is_open()) {
        while (getline(inputFile, storedUsername)) {
            getline(inputFile, storedPassword);

            if (storedUsername == enteredUsername && storedPassword == enteredPassword) {
                inputFile.close();
                return true;
            }
        }
        inputFile.close();
    }
    return false;
}

TCPServer::TCPServer() : listening(-1), clientSocket(-1) {}

bool TCPServer::initServer(int port) {
    listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        cerr << "Unable to create socket! Cancel..." << endl;
        return false;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(listening, reinterpret_cast<sockaddr *>(&hint), sizeof(hint)) == -1)
    {
        cerr << "Binding failed..." << endl;
        return false;
    }

    if (listen(listening, SOMAXCONN) == -1)
    {
        cerr << "Listening failed..." << endl;
        return false;
    }

    return true;
}

void TCPServer::start() {
    // Implementation here
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);

    clientSocket = accept(listening, reinterpret_cast<sockaddr *>(&client), &clientSize);

    displayClientInfo(client);
    processCase();
}

void TCPServer::handleRegistration(int clientSocket) {
    // Implementation here
    char usernameBuffer[1024] = {0};
    char passwordBuffer[1024] = {0};

    recv(clientSocket, usernameBuffer, sizeof(usernameBuffer), 0);
    recv(clientSocket, passwordBuffer, sizeof(passwordBuffer), 0);

    string username(usernameBuffer);
    string password(passwordBuffer);

    bool status = auth.isUserRegistered(username);
    status = !status && auth.registerUser(username, password);
    // const char* response = status ? "Registration successful." : "Registration failed.";
    const char* response;
    if (status) {
        response = "Registration successful.";
    } else {
        response = "Username already exists. Choose a different username.";
    }
    send(clientSocket, response, strlen(response), 0);

}

void TCPServer::handleLogin(int clientSocket) {
    // Implementation here
    char usernameBuffer[1024] = {0};
    char passwordBuffer[1024] = {0};

    recv(clientSocket, usernameBuffer, sizeof(usernameBuffer), 0);
    recv(clientSocket, passwordBuffer, sizeof(passwordBuffer), 0);
    // read(clientSocket, usernameBuffer, 1024);
    // read(clientSocket, passwordBuffer, 1024);

    string username(usernameBuffer);
    string password(passwordBuffer);

    bool status = auth.isLoggedIn(username, password);
    const char* response = status ? "Login successful." : "Login failed.";
    send(clientSocket, response, strlen(response), 0);
}

void TCPServer::processCase() {
    // Implementation here
    char buffer[4096];
    while(true)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == -1)
        {
            cerr << "Error while receiving data!" << endl;
            return;
        }
        if (bytesReceived == 0)
        {
            cout << "Client has disconnected" << endl;
            return;
        }

        int choice = stoi(string(buffer, 0, bytesReceived));
        // string userInput;
    
        switch (choice)
        {
            case 1:
                {
                    thread receivingThread(&TCPServer::receiveData, this);
                    while (true){
                        string userInput;
                        cout << "You: ";
                        getline(cin, userInput);
                        sendData(userInput);
                    }
                    break;
                }
            case 2:
                {
                    if (clientSocket < 0) {
                    cerr << "Error in accepting connection." << endl;
                    // continue;
                    }
                    cout <<"New client connected." << endl;
                    handleClient(clientSocket);
                    break;
                }

            default:
                return;
        }
    }
}

void TCPServer::receiveData() {
    // Implementation here
    char buffer[4096];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == -1) {
            cerr << "Error while receiving data!" << endl;
            break;
        }
        if (bytesReceived == 0) {
            cout << "Client has disconnected" << endl;
            break;
        }
        cout << "\nClient: " << string(buffer, 0, bytesReceived) << endl;
    }
}

void TCPServer::sendData(const std::string& data) {
    // Implementation here
    send(clientSocket, data.c_str(), data.size() + 1, 0);

}

void TCPServer::handleClient(int clientSocket) {
    // Implementation here
    // string buffer;
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    int option = stoi(string(buffer, 0, bytesReceived));
    // recv(clientSocket, buffer, sizeof(buffer));
    // int option = atoi(buffer);

    if (option == 1) {
        // cout <<"waiting for registration" <<endl;
        // Register
        handleRegistration(clientSocket);
        
    } else if (option == 2) {
        // Login
        handleLogin(clientSocket);
    }
}

void TCPServer::closeConnection() {
    // Implementation here
    close(clientSocket);    
    close(listening);
}

void TCPServer::displayClientInfo(const sockaddr_in& client) {
    // Implementation here
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    if (getnameinfo(reinterpret_cast<const sockaddr *>(&client), sizeof(client),
                    host, NI_MAXHOST, service, NI_MAXSERV, 0))
    {
        cout << host << " connected on port " << service << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on port " << ntohs(client.sin_port) << endl;
    }
}




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
// void TCPServer::acceptConnections() {
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

