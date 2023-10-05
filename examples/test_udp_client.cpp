#include <iostream>
#include <string>
using namespace std;

#include "nanonet"

int main() {
    // create a socket
    nanonet::udp_socket socket;

    // connect to remote
    socket.set_remote("127.0.0.1", 8888);

    // send buffer
    char buf[4096]{};
    
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
        nanonet::addr_port remote = socket.receive(buf, 4095);
        std::cout << "server> " << buf << std::endl;
    
    } // while

    // close socket
    socket.close();

    return 0;
}