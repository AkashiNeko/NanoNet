#include <iostream>
#include <string>

#include "nanonet.h"
using namespace nanonet;

int main() {
    // create a socket and bind the port
    udp_socket socket(8888);

    // receive buffer
    char buf[4096]{};

    // cyclically receive message
    while (true) {

        // receive message from client
        addr_port ipport = socket.receive(buf, 4095);

        // get data
        std::string msg = buf;
        std::cout << "client: " << msg << std::endl;

        // set remote client
        socket.remote(ipport);

        // reply client
        socket.send(msg);

    } // while

    return 0;
}