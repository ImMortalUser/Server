// tcpsocket.h

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
    void connect(const std::string& ipAddress, unsigned short port);
    void send(const std::string& message);
    static void initializeWinsock();
    std::string receive();
    static void cleanup(); // Обновленный метод
private:
    SOCKET socket_; // Поле для хранения сокета
};

#endif // TCPSOCKET_H
