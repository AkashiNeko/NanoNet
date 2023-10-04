#include <iostream>
#include <string>

#include "nanonet"

int main() {

    // create a socket
    nanonet::tcp_socket socket("127.0.0.1", 8888);

    // connect to server
    socket.connect();

    // receive buffer
    char buf[4096];

    // cyclically send message
    while (true) {

        // get message by user input
        std::cout << "client> ";
        std::string msg;
        getline(std::cin, msg);
        if (msg.empty()) continue;

        // send message to server
        socket.send(msg);

        // client quit
        if (msg == "quit") break;

        // receive from server
        socket.receive(buf, 4095);
        std::cout << "server> " << buf << std::endl;
    }

    // close the socket
    socket.close();

    return 0;
}