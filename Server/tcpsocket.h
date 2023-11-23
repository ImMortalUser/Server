// TCPSocket.h

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
    void bindAndListen(unsigned short port);
    int acceptConnection(); // Изменили тип на int

    static void initializeNetworking(); // Общая инициализация для Windows и Linux

    static std::string receiveFromClient(int clientSocket); // Изменили тип на int
    static void sendToClient(int clientSocket, const std::string& message); // Изменили тип на int
    static std::string getClientIP(int clientSocket); // Изменили тип на int
    static void closeConnection(int clientSocket); // Изменили тип на int

private:
    int socket_; // Изменили тип на int
    int getErrorCode();
};

#endif // TCPSOCKET_H
