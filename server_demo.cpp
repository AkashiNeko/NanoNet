#include <iostream>
#include <string>
#include <signal.h>

#include "nanonet.h"

using namespace std;
using namespace nanonet;

int main() {

    signal(SIGCHLD, SIG_IGN);

    ServerSocket serverSocket(8888);
    while (true) {
        Socket socket = serverSocket.accept();
        if (fork() == 0) {
            // child
            cout << "process start.. pid = " << getpid() << endl;
            serverSocket.close();

            string remote_ip = socket.getRemoteStrAddr();
            port_t port = socket.getRemotePort();
            string remote_name = "[" + remote_ip + ":" + to_string(port) + "]";

            cout << "remote " << remote_name << " connected." << endl;
            char buf[4096] = {0};
            while (socket.receive(buf, 4095)) {
                string msg = buf;
                cout << remote_name << "# " << msg << endl;
                if (msg == "quit") break;
                socket.send(msg);
            }
            cout << "remote " << remote_name << " closed." << endl;
            socket.close();
            cout << "process exit: pid = " << getpid() << endl;
            return 0;
        }
        socket.close();
    }
    return 0;
}