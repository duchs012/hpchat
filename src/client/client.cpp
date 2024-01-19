// Client.cpp
#include "client.h"

Client::Client() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Unable to create socket! Cancel..." << endl;
        // Handle error accordingly
    }
}

Client::~Client() {
    close(clientSocket);
    receivingThread.join();
}

bool Client::connectToServer(const std::string& ipAddress, int port) {
    // Implementation here
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    int connectRes = connect(clientSocket, (sockaddr *)&hint, sizeof(hint));
    if (connectRes == -1) {
        cerr << "Connection to server failed" << endl;
        close(clientSocket);
        return false;
    }
    return true;
}

void Client::receiveData() {
    // Implementation here
    char buffer[4096];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, 4096, 0);
        if (bytesReceived == -1) {
            cerr << "Error while receiving data!" << endl;
            break;
        }
        if (bytesReceived == 0) {
            cout << "Server disconnected" << endl;
            break;
        }
        cout << "\nServer: " << string(buffer, 0, bytesReceived) << endl;
    }
}

void Client::displayMenu() {
    // cout << "----- Services -----" << endl;
    // cout << "1. Start Communication" << endl;
    // cout << "2. Login and Registration" << endl;
    // cout << "3. Exit" << endl;
    // cout << "---------------" << endl;

    cout << "----- Services -----" << endl;
    cout << "1. Start Communication" << endl;
    cout << "2. Login and Registration" << endl;
    cout << "3. Send File" << endl; // Added option to send file
    cout << "4. Exit" << endl;
    cout << "---------------" << endl;
}

void Client::startCommunication() {
    // Implementation here
    receivingThread = thread(&Client::receiveData, this);

    char buffer[4096];
    while (true) {
        string userInput;
        cout << "You: ";
        getline(cin, userInput);
        send(clientSocket, userInput.c_str(), userInput.size() + 1, 0);
    }
}

void Client::sendFile(const std::string& filePath) {
    std::ifstream fileStream(filePath, std::ios::binary);
    if (!fileStream) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    // Extract file name from path
    std::size_t found = filePath.find_last_of("/");
    std::string fileName = filePath.substr(found + 1);

    // Send file name
    send(clientSocket, fileName.c_str(), fileName.size() + 1, 0);

    // Send file content
    char buffer[4096];
    while (!fileStream.eof()) {
        fileStream.read(buffer, sizeof(buffer));
        send(clientSocket, buffer, fileStream.gcount(), 0);
    }

    fileStream.close();
    std::cout << "File sent successfully" << std::endl;
}




void Client::closeConnection() {
    // Implementation here
    close(clientSocket);
    receivingThread.join();
}

void Client::handleUserInteraction() {
    // Implementation here
    int option;
    cout << "1: Register" << endl << "2: Login" << endl << "Your Choice: ";
    cin >> option;
    cin.ignore();

    // send(clientSocket, to_string(option).c_str(), strlen(to_string(option).c_str()), 0);
    send(clientSocket, to_string(option).c_str(), to_string(option).size() + 1, 0);

    if (option == 1 || option == 2) {
        char username[1024];
        char password[1024];

        cout << "Enter Username: ";
        cin.getline(username, sizeof(username));
        send(clientSocket, username, strlen(username), 0);

        cout << "Enter Password: ";
        cin.getline(password, sizeof(password));
        send(clientSocket, password, strlen(password), 0);
        
        char buffer[1024] = {0};
        read(clientSocket, buffer, 1024);
        cout << buffer << endl;
    } else {
        cout << "Invalid option" << endl;
    }
}

void Client::runMenu() {
    // Implementation here
    int choice;
    connectToServer("10.188.9.15", 54000);
    do {
        std::string filePath;
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Clear input buffer
        send(clientSocket, to_string(choice).c_str(), to_string(choice).size() + 1, 0);
        switch (choice) {
            case 1:
                startCommunication();
                closeConnection();
                break;
            // case 2:
            //     handleUserInteraction();
            //     // closeConnection();
            //     break;
            // case 3:
            //     cout << "Exiting..." << endl;
            //     break;
            case 3:
            std::cout << "Enter the path of the file to send: ";
            std::getline(std::cin, filePath); // Use filePath inside the loop
            sendFile(filePath);
            break;
                break;
            case 4:
                std::cout << "Exiting..." << std::endl;
                break;
            default:
                cout << "Invalid choice. Please enter a valid option." << endl;
                break;
        }
    } while (choice != 4);
}


