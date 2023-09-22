#include <iostream>
#include <string>

#include "nanonet.h"

int main() {
    // create a socket and bind the port
    nanonet::udp_socket socket(8888);

    // receive buffer
    char buf[4096]{};

    // cyclically receive message
    while (true) {

        // receive message from client
        nanonet::addr_port ipport = socket.receive(buf, 4095);

        // get data
        std::string msg = buf;
        std::cout << "[" << ipport.to_string() << "]# " << msg << std::endl;

        // client quit?
        if (msg == "quit") {
            std::cout << "client " << ipport.to_string() << " quit" << std::endl;
            continue;
        }

        // set remote client
        socket.remote(ipport);

        // reply client
        socket.send(msg);

    } // while

    return 0;
}