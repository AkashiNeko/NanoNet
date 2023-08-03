#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "nanonet.h"

using namespace std;
using namespace nanonet;

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

const string WWW_ROOT = "./wwwroot";

int main() {

    ServerSocket serverSocket(8080);

    while (true) {
        Socket socket = serverSocket.accept();

        string remote_ip = socket.getRemoteStrAddr();
        port_t port = socket.getRemotePort();
        string remote_name = "[" + remote_ip + ":" + to_string(port) + "]";

        cout << "remote " << remote_name << " connected." << endl;
        char buf[4096] = {0};
        size_t len, i = 0;
        while (len = socket.receive(buf, 4096)) {
            for (i = 0; i < len && buf[i] != '\r'; ++i);
            string line(buf, i);
            auto split_line = split(line, ' ');
            if (split_line.size() == 3) {
                string path = WWW_ROOT + split_line[1];
                if (path.back() == '/')
                    path += "index.html";
                cout << "path: " << path << endl;

                string file_string;
                ifstream file(path.c_str(), ios::in);
                if (file.is_open()) {
                    while (!file.eof()) {
                        string htmlLine;
                        getline(file, htmlLine);
                        file_string += htmlLine;
                    }
                    file.close();
                }

                string ret = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
                ret += to_string(file_string.size()) + "\r\n\r\n" + file_string;
                socket.send(ret);
                cout << "------------------ ret -----------------" << endl;
                cout << ret << endl;
                cout << "----------------------------------------" << endl;
            }
        }
        cout << "remote " << remote_name << " closed." << endl;
    }
    return 0;
}