#include <iostream>
#include <string>

#include "nanonet"

int main() {
    // create a socket and bind the port
    nanonet::udp_socket socket(8888);

    // receive buffer
    char buf[4096]{};

    // cyclically receive message
    while (true) {

        // receive message from client
        auto ipport = socket.receive(buf, 4095);

        // get message from buf
        std::string msg = buf;

        // client sends a "quit" message
        if (msg == "quit") {
            std::cout << "client " << ipport.to_string() << " quit" << std::endl;
            continue;
        }

        std::cout << "[" << ipport.to_string() << "]# " << msg << std::endl;

        // set remote client
        socket.remote(ipport);

        // reply client
        socket.send(msg);

    } // while

    return 0;
}