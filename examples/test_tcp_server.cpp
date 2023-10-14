#include <iostream>
#include <cstring>

#include "nanonet"

int main() {

    // create a server socket
    nanonet::ServerSocket server("127.0.0.1", 8888);
    std::cout << "server start, wait for client.." << std::endl;

    while (true) {
        // wait for a client connection
        nanonet::TCPSocket socket = server.accept();

        // get remote addr & port
        std::string remoteName = socket.getRemoteAddrPort().toString();
        std::cout << "client " << remoteName << " connected" << std::endl;

        // receive buffer
        char buf[4096]{};

        // cyclically receive message
        while (true) {

            // receive message from client
            if (socket.receive(buf, 4096) == 0) {
                std::cout << "client " << remoteName << " exit" << std::endl;
                break;
            }

            // get message from buf
            std::string msg = buf;

            // client sends a "quit" message
            if (msg == "quit") {
                std::cout << "client " << remoteName << " quit" << std::endl;
                break;
            }

            // show message
            std::cout << "[" << remoteName << "]# " << msg << std::endl;

            // reply client
            socket.send(msg);
        }

        // parent process closes the socket
        socket.close();
    }

    // close the server socket
    server.close();

    return 0;
}