// http/requests.cpp

#include "nanonet/http/requests.h"

namespace nanonet {

// send method
HttpRespond Requests::methods(const char* method, const std::string& url, const std::string& body) {
    HttpRequest httpRequest(method, url);
    if (!body.empty())
        httpRequest.setBody(body);
    return send(httpRequest);
}

// get addr and port from request
AddrPort Requests::getAddrPortFromHost(const std::string& host, bool useSSL) {
    int slashPos = host.find(':');
    Port port = useSSL ? 443 : 80;
    if (slashPos != std::string::npos)
        port = static_cast<in_port_t>(std::stoi(host.substr(slashPos + 1)));
    Addr addr(host.substr(0, slashPos));
    return AddrPort(addr, port);
}

// send request
HttpRespond Requests::send(HttpRequest request, int timeout) {
    std::string host = request.getHost();
    AddrPort addrPort = getAddrPortFromHost(host, request.usingSSL());

    // connnect to server
    Socket socket;
    socket.connect(addrPort);

    // send
    socket.send(request.toString());

    debug << "[http] send http request to \'"
        << host << '\'' << std::endl;

    const int BUF_SIZE = 4096;
    char buffer[BUF_SIZE]{};
    socket.setReceiveTimeout(timeout, 0);

    // receive
    int receiveLength = socket.receive(buffer, BUF_SIZE - 1);

    if (receiveLength < 0) {
        // receive failed
        warn << "[http] receive from \'"
            << host << "\' timeout" << std::endl;
        socket.close();
        return {};
    } else if (receiveLength == 0) {
        // server not responding
        warn << "[http] server not responding" << std::endl;
        socket.close();
        return {};
    }
    buffer[receiveLength] = '\0';

    HttpRespond respond;
    HttpAssembler<HttpRespond> assembler(respond);
    bool receiveDone = assembler.append(buffer);
    while (!receiveDone && receiveLength > 0) {
        socket.setReceiveTimeout(5, 0);
        buffer[0] = '\0';
        receiveLength = socket.receive(buffer, BUF_SIZE - 1);
        if (receiveLength >= 0)
            buffer[receiveLength] = '\0';
        receiveDone = assembler.append(buffer);
    }

    debug << "[http] receive from \'" << host
        << "\', text length = " << respond.size() << std::endl;
    socket.close();
    return respond;
}

}  // namespace nanonet