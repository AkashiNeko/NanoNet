#include <iostream>
#include "nanonet.h"

using namespace std;
using namespace nanonet;

int main() {
    UdpSender udp("127.0.0.1", 8888);
    udp.send("Hello, world!");
    return 0;
}