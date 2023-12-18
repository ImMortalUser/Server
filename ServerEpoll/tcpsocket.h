#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <iostream>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
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
    int acceptConnection();
    static void initializeNetworking();

    static std::string receiveFromClient(int clientSocket);
    static void sendToClient(int clientSocket, const std::string& message);
    static std::string getClientIP(int clientSocket);
    static void closeConnection(int clientSocket);

    int getSocketFD() const; // Добавлен метод для получения файлового дескриптора сокета

private:
    int socket_;
    int getErrorCode();
};

#endif // TCPSOCKET_H
