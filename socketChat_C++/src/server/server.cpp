#include "server.h"

Client::Client(int socket, const std::string& name, const std::string& roomName) : socket(socket), name(name), roomName(roomName) {}

int Client::getSocket() const {
    return socket;
}

const std::string& Client::getName() const {
    return name;
}

const std::string& Client::getRoomName() const {
    return roomName;
}

Server::Server() {
    // ... (constructor implementation)
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Can't create the socket" << std::endl;
        return;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&hint, sizeof(hint)) == -1) {
        std::cerr << "Bind failed!" << std::endl;
        close(serverSocket);
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == -1) {
        std::cerr << "Listen failed!" << std::endl;
        close(serverSocket);
        return;
    }
}

Client* Server::findClientByName(const std::string& name) {
    // ... (findClientByName implementation)
    std::lock_guard<std::mutex> guard(clientsMutex);
    auto it = std::find_if(clients.begin(), clients.end(), [this,name](const Client& client) {
        return trim(client.getName()) == trim(name);
    });
    return (it != clients.end()) ? &(*it) : nullptr;
}

void Server::handlePrivateMessage(const std::string& senderName, const std::string& receiverName, const std::string& message) {
    // ... (handlePrivateMessage implementation)
    // Lock the clientsMutex to ensure thread safety
    std::lock_guard<std::mutex> guard(clientsMutex);

    // Find the sender in the clients array
    Client* sender = nullptr;
    for (auto& client : clients) {
        if (client.getName() == senderName) {
            sender = &client;
        }
    }

    // If the sender is found, iterate through all clients to find the receiver
    if (sender) {
        for (auto& client : clients) {
            if (client.getName() == receiverName) {
                
                //int sendResult = sendPrivateMessage(client.getSocket(), message);Sai
                send(client.getSocket(),message.c_str(),message.size()+1,0);
                
                //std::cout << "Send result: " << sendResult << std::endl;
                // Print the private message sent from sender to receiver
                // std::cout << "Private message sent from " << senderName << " to " << receiverName << ": " << message << std::endl;
                return;  // Exit the function after sending the message to one receiver
            }
        }
        // Print an error message if the receiver is not found
        std::cerr << "Error: Receiver not found!" << std::endl;
    } else {
        // Print an error message if the sender is not found
        std::cerr << "Error: Sender not found!" << std::endl;
    }
}

void Server::start() {
    // ... (start implementation)
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
        if (clientSocket == -1) {
            std::cerr << "Error when create the socket from client" << std::endl;
            continue;
        }

        // Receive room name from client
        std::string roomName = receiveString(clientSocket);
        if (roomName.empty()) {
            close(clientSocket);
            continue;
        }

        // Receive client name from client
        std::string clientName = receiveString(clientSocket);
        if (clientName.empty()) {
            close(clientSocket);
            continue;
        }

        std::lock_guard<std::mutex> guard(clientsMutex);
        clients.push_back(Client(clientSocket, clientName, roomName));

        std::thread clientThread(&Server::handleClient, this, clientSocket, clientName, roomName);
        clientThread.detach();
    }
}

std::string Server::trim(const std::string& str) {
    // ... (trim implementation)
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");

    if (first == std::string::npos || last == std::string::npos)
        return "";
    else
        return str.substr(first, last - first + 1);
}

std::string Server::receiveString(int clientSocket) {
    // ... (receiveString implementation)
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, 4096, 0);
    if (bytesReceived <= 0) {
        std::cerr << "Error: Received data from client failed!" << std::endl;
        return "";
    }
    return std::string(buffer, 0, bytesReceived);
}

void Server::handleClient(int clientSocket, const std::string& clientName, const std::string& roomName) {
    // ... (handleClient implementation)
    char buffer[4096];
    int bytesReceived;
    while (true) {
        bytesReceived = recv(clientSocket, buffer, 4096, 0);
        if (bytesReceived <= 0) {
                std::cerr << "Client " << clientName << " offline!" << std::endl;
                std::lock_guard<std::mutex> guard(clientsMutex);
                clients.erase(std::remove_if(clients.begin(), clients.end(), [clientSocket](const Client& client) {
                    return client.getSocket() == clientSocket;
                }), clients.end());
                close(clientSocket);
                return;
            
        }

        std::string receivedMessage(buffer, 0, bytesReceived);


        // Check the user that want to send the privare Message
        if (receivedMessage.size() > 3 && receivedMessage.substr(0, 3) == "/pm") {
            //handleSendPrivateMessage(receivedMessage,clientSocket,bytesReceived,clientName, buffer);
            //format "/pm receiverName message"
            size_t spacePos = receivedMessage.find(' ', 4);
            if (spacePos != std::string::npos) {
                std::string receiverName = trim(receivedMessage.substr(4, spacePos - 4));
                std::string privateMessage = trim(receivedMessage.substr(spacePos + 1));
        
                // Handle to send the private file
                while (true) {
                    if (privateMessage.substr(0, 4) == "exit") {
                        std::string NotificationExitPrivateChat ="\033[1;32mYou exited the private chat and came back to the room chat hiiiiihihihihihihihihihihi\033[0m";
                        for (const auto& client : clients) {
                            if ( client.getSocket()== clientSocket) {
                                send(client.getSocket(), NotificationExitPrivateChat.c_str(), NotificationExitPrivateChat.size() + 1, 0);
                            }
                        }
                        break;
                    } else {
                        std::string Private = "\033[1;31mPrivate chat from>>\033[0m";
                        std::string clientNameColor = "\033[1;34m" + clientName + "\033[0m";
                        std::string messageColor = "\033[1;37m" + privateMessage + "\033[0m";

                        handlePrivateMessage(clientName, receiverName, Private + " " + clientNameColor + ": " + messageColor);
                        bytesReceived = recv(clientSocket, buffer, 4096, 0);
                        //handleClientOffline(clientSocket,clientName,roomName,bytesReceived);
                        if (bytesReceived <= 0) {
                            std::cerr << "Client " << clientName << " offline!" << std::endl;
                            return;
                        }
                        privateMessage = std::string(buffer, 0, bytesReceived);
                    }
                }
            } else {
                std::string Notification1 ="\033[1;33m Error: Incorrect private message format!\n Correct is : /pm + User's name + your message!\n The list users are online : \n\033[0m";
                std::string clientNamesList1;
                for (const auto& client : clients) {
                    clientNamesList1 += client.getName() + "\n"; 
                }
                std::string A=Notification1 + clientNamesList1;
                for (const auto& client : clients) {
                    if ( client.getSocket()== clientSocket) {
                        send(client.getSocket(), A.c_str(), A.size() + 1, 0);
                    }
                }
            }
        } 
        //send file huihu dw
        else if(receivedMessage.substr(0, 9) == "/sendfile") {
            std ::string SendFile ="File is being sent from ";
            std::cout<< SendFile <<std::endl;
        }
        
        else {
            // Handle to send message to the same roomName
            std::string groupMessage = "\033[1;34m" + clientName +"\033[0m" + ": " + receivedMessage;
            std::lock_guard<std::mutex> guard(clientsMutex);
            for (const auto& client : clients) {
                if (client.getRoomName() == roomName && client.getSocket() != clientSocket) {
                    std::string tenPhong = "\033[1;35m" + client.getRoomName()+ "\033[0m" +">>" + groupMessage ;
                    send(client.getSocket(), tenPhong.c_str(), tenPhong.size() + 1, 0);
                }
            }
        }
    }
}
