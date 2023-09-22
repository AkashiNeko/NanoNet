#include <iostream>
#include <cstring>
#include <signal.h>

#include "nanonet.h"

int main() {

    // ignore the SIGCHLD signal, let the child process reap itself
    signal(SIGCHLD, SIG_IGN);

    // create a server socket
    nanonet::server_socket ss(8888);
    std::cout << "server start, wait for client.." << std::endl;

    while (true) {
        // wait for a client connection
        nanonet::socket sock = ss.accept();

        // create a child process, the parent process continues to listen
        if (!fork()) {

            // close the server socket of the child process
            ss.close();

            // get remote ip & port
            std::string remoteName = sock.remote_addrport().to_string();
            std::cout << "client " << remoteName << " connected" << std::endl;

            // receive buffer
            char buf[4096]{};

            // cyclically receive message
            while (true) {

                // receive message from client
                sock.receive(buf, 4096);
                std::string msg = buf;
                std::cout << "[" << remoteName << "]# " << msg << std::endl;

                // client quit?
                if (msg == "quit") {
                    std::cout << "client " << remoteName << " closed" << std::endl;
                    break;
                }

                // reply client
                sock.send(msg);
            }

            // close socket
            sock.close();

            // child process exits
            return 0;
        }

        // parent process closes the socket
        sock.close();
    }

    // close the server socket
    ss.close();

    return 0;
}