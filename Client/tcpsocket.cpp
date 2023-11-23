// TCPSocket.cpp

#include "tcpsocket.h"
#include <iostream>
#include <chrono>
#include <stdexcept> // Для использования std::runtime_error

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

void TCPSocket::cleanup() {
    WSACleanup();
}

#else
// Для Linux

void TCPSocket::initializeNetworking() {

}

void TCPSocket::cleanup() {

}

#endif


void TCPSocket::connect(const std::string& ipAddress, unsigned short port) {
    sockaddr_in serverAddrIPv4{};
    serverAddrIPv4.sin_family = AF_INET;
    serverAddrIPv4.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &(serverAddrIPv4.sin_addr));

    if (::connect(socket_, reinterpret_cast<sockaddr*>(&serverAddrIPv4), sizeof(serverAddrIPv4)) == -1) {
        int errorCode = getErrorCode();
        throw std::runtime_error("Connect failed with error: " + std::to_string(errorCode));
    }
}

void TCPSocket::send(const std::string& message) {
    const char* data = message.c_str();
    int totalSent = 0;
    int remaining = static_cast<int>(message.length());

    while (remaining > 0) {
        int sent = ::send(socket_, data + totalSent, remaining, 0);
        if (sent == -1) {
            throw std::runtime_error("Failed to send data");
        }

        totalSent += sent;
        remaining -= sent;
    }
}


std::string TCPSocket::receive() {
    char buffer[1024];
    int bytesRead = ::recv(socket_, buffer, sizeof(buffer), 0);

    if (bytesRead > 0) {
        return std::string(buffer, bytesRead);
    } else if (bytesRead == 0) {
        // Соединение закрыто
        throw std::runtime_error("Connection closed.");
    } else {
        // Ошибка при приеме данных
        int error = getErrorCode();
        throw std::runtime_error("Error receiving data. Error code: " + std::to_string(error));
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

