#include "client.h"

int main() {
    Client client;

    if (client.connectToServer("10.188.9.19", 54000)) {
        /// void login (){goto :abc}
        //abc:
        std::string roomName;
        std::cout << "Enter the ID or RoomName: ";
        std::getline(std::cin, roomName);
        client.sendToServer(roomName);

        client.startReceiving();

        std::string clientName;
        std::cout << "Enter Your name: ";
        std::getline(std::cin, clientName);

        // Sử dụng hàm trim thông qua đối tượng client
        clientName = client.trim(clientName);

        client.sendToServer(clientName);

        while (true) {
            std::string userInput;
            std::getline(std::cin, userInput);

            // Sử dụng hàm trim thông qua đối tượng client
            userInput = client.trim(userInput);

            client.sendToServer(userInput);
        }
    }

    return 0;
}
