// #include "server.h"

// int main() {
//     TCPServer server;

//     // Specify the range of ports to attempt binding
//     int port = 12345; // Replace with the desired port number
//     if (!server.initServer(port)) {
//         std::cerr << "Server initialization failed!" << std::endl;
//         return -1;
//     }

//     server.start();
//     return 0;
// }



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
