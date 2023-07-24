#include <iostream>
#include "nanonet.h"

using namespace std;
using namespace nanonet;

int main() {
    UdpServer server(8888);
    while (true) {
        auto [hostPort, msg] = server.wait();
        cout << "[" << hostPort.to_string() << "]# " << msg << endl;
    }
    return 0;
}