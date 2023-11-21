#include <iostream>
#include <string>

#include "nanonet.h"

int main() {
    // create a socket
    nanonet::UdpSocket socket;

    // bind the address & port
    socket.bind(8888);

    // receive buffer
    char buf[4096]{};

    // cyclically receive message
    while (true) {

        // receive message from client
        auto addrPort = socket.receive(buf, 4095);

        // get message from buf
        std::string msg = buf;

        // client sends a "quit" message
        if (msg == "quit") {
            std::cout << "client " << addrPort.toString() << " quit" << std::endl;
            continue;
        }

        std::cout << "[" << addrPort.toString() << "]# " << msg << std::endl;

        // set remote client
        socket.setRemote(addrPort);

        // reply client
        socket.send(msg);

    } // while

    return 0;
}