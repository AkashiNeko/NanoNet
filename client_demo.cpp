#include <iostream>
#include <string>
using namespace std;

#include "nanonet.h"
using namespace nanonet;

inline string input(string tips) {
    cout << tips << ends;
    string ret;
    getline(cin, ret);
    return ret;
}

int main() {
    Socket socket("127.0.0.1", 8888);
    socket.connect();
    while (true) {
        string msg = input("client> ");
        socket.send(msg);

        char buffer[4096];
        if (socket.receive(buffer, 4095) == 0)
            break;
        
        cout << buffer << endl;
    }
    socket.close();
    return 0;
}