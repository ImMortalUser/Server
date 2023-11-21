// TCPSocket.h

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

class TCPSocket {
public:
    TCPSocket();
    ~TCPSocket();
    void bindAndListen(unsigned short port);
    SOCKET acceptConnection();


    static void initializeWinsock();

    static std::string receiveFromClient(SOCKET clientSocket);
    static void sendToClient(SOCKET clientSocket, const std::string& message);
    static std::string getClientIP(SOCKET clientSocket);
    static void closeConnection(SOCKET clientSocket);

private:
    SOCKET socket_;
};

#endif // TCPSOCKET_H
