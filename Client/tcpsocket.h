// tcpsocket.h

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <iostream>
#include <string>

#ifdef _WIN32
// Для Windows
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Линковка с библиотекой ws2_32.lib
#else
// Для Linux
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

class TCPSocket {
public:
    TCPSocket();
    ~TCPSocket();
    void connect(const std::string& ipAddress, unsigned short port);
    void send(const std::string& message);
    static void initializeNetworking();
    std::string receive();
    static void cleanup(); // Обновленный метод
private:
    int socket_; // Поле для хранения сокета
    int getErrorCode();
};

#endif // TCPSOCKET_H
