#include <iostream>
#include <string>
using namespace std;

#include "nanonet.h"
using namespace nanonet;

int main() {
    // create a socket
    udp_socket socket;

    // connect to remote
    socket.remote("127.0.0.1", 8888);

    // send buffer
    char buf[4096]{};
    
    // cyclically send message
    while (true) {

        // get message by user input
        std::cout << "client> ";
        std::string msg;
        getline(std::cin, msg);

        // send message to server
        socket.send(msg);

        // quit ?
        if (msg == "quit") break;

        // receive from server
        addr_port remote = socket.receive(buf, 4095);
        std::cout << "server> " << buf << std::endl;
    
    } // while

    // close socket
    socket.close();

    return 0;
}