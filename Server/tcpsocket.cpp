// TCPSocket.cpp

#include "TCPSocket.h"
#include <iostream>
#include <chrono>

TCPSocket::TCPSocket() {
    initializeWinsock();
    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET) {
        throw std::runtime_error("Failed to create socket");
    }
}

TCPSocket::~TCPSocket() {
    closesocket(socket_);
}

void TCPSocket::initializeWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("Failed to initialize Winsock");
    }
}

void TCPSocket::bindAndListen(unsigned short port) {
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        throw std::runtime_error("Bind failed with error: " + std::to_string(WSAGetLastError()));
    }

    if (listen(socket_, SOMAXCONN) == SOCKET_ERROR) {
        throw std::runtime_error("Listen failed with error: " + std::to_string(WSAGetLastError()));
    }
}

SOCKET TCPSocket::acceptConnection() {
    SOCKET clientSocket = accept(socket_, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        throw std::runtime_error("Accept failed with error: " + std::to_string(WSAGetLastError()));
    }

    return clientSocket;
}

void TCPSocket::closeConnection(SOCKET clientSocket) {
    closesocket(clientSocket);
}

std::string TCPSocket::receiveFromClient(SOCKET clientSocket) {
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

void TCPSocket::sendToClient(SOCKET clientSocket, const std::string& message) {
    int bytesSent = ::send(clientSocket, message.c_str(), static_cast<int>(message.length()), 0);
    if (bytesSent == SOCKET_ERROR) {
        // Ошибка при отправке данных
        //throw std::runtime_error("Send failed with error: " + std::to_string(WSAGetLastError())); // раскомментить в будущем
    }
}

std::string TCPSocket::getClientIP(SOCKET clientSocket) {
    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);

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
