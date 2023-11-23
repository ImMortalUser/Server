// TCPSocket.cpp

#include "tcpsocket.h"
#include <iostream>
#include <chrono>

TCPSocket::TCPSocket() {
    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == -1) {
        throw std::runtime_error("Failed to create socket");
    }
}

TCPSocket::~TCPSocket() {
#ifdef _WIN32
    closesocket(socket_);
#else
    close(socket_);
#endif
}

#ifdef _WIN32

void TCPSocket::initializeNetworking() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("Failed to initialize Winsock");
    }
}

#else
// Для Linux

void TCPSocket::initializeNetworking() {

}

#endif

void TCPSocket::bindAndListen(unsigned short port) {
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        throw std::runtime_error("Bind failed with error: " + std::to_string(getErrorCode()));
    }

    if (listen(socket_, SOMAXCONN) == -1) {
        throw std::runtime_error("Listen failed with error: " + std::to_string(getErrorCode()));
    }
}

int TCPSocket::acceptConnection() {
    int clientSocket = accept(socket_, nullptr, nullptr);
    if (clientSocket == -1) {
        throw std::runtime_error("Accept failed with error: " + std::to_string(getErrorCode()));
    }

    return clientSocket;
}

void TCPSocket::closeConnection(int clientSocket) {
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}

std::string TCPSocket::receiveFromClient(int clientSocket) {
    char buffer[1024];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead > 0) {
        return std::string(buffer, bytesRead);
    } else if (bytesRead == 0) {
        // Соединение закрыто
        //throw std::runtime_error("Connection closed"); //каждый раз обрабатывать recv, в будущем раскомментировать
        return "";
    } else {
        // Ошибка при приеме данных
        //throw std::runtime_error("Receive failed with error: " + std::to_string(WSAGetLastError())); // тоже самое, что выше
        return "";
    }
}

void TCPSocket::sendToClient(int clientSocket, const std::string& message) {
    int bytesSent = ::send(clientSocket, message.c_str(), static_cast<int>(message.length()), 0);
    if (bytesSent == -1) {
        // Ошибка при отправке данных
        //throw std::runtime_error("Send failed with error: " + std::to_string(WSAGetLastError())); // раскомментить в будущем
    }
}

std::string TCPSocket::getClientIP(int clientSocket) {
    sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    if (getsockname(clientSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen) == 0) {
        char ipStr[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, sizeof(ipStr)) != nullptr) {
            return std::string(ipStr);
        } else {
            // Ошибка при преобразовании IP-адреса в строку
            throw std::runtime_error("Error converting IP address to string");
        }
    } else {
        // Ошибка при получении информации о сокете
        throw std::runtime_error("Error getting socket information");
    }
}

int TCPSocket::getErrorCode()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}
