#include <iostream>
#include <thread>
#include "TCPSocket.h"  // Подключаем ваш класс TCPSocket
#include <cstdlib>

const unsigned short SERVER_PORT = 1414;
const std::string SERVER_IP = "5.187.83.11";
static const std::string CLEAR_SCREEN_COMMAND = "<clrscr>";
static const int MAX_INPUT_LENGTH = 256;
static const int RETRY_DELAY_MS = 500;
static const int MAX_CONNECTION_ATTEMPTS = 5;




static void receive(TCPSocket& socket) {
    try {
        while (true) {
            std::string receivedMessage = socket.receive();
            if (!receivedMessage.empty()) {
                if (receivedMessage.find(CLEAR_SCREEN_COMMAND) != std::string::npos) {
                    system("cls");
                    std::cout << receivedMessage.substr(CLEAR_SCREEN_COMMAND.length()) << std::endl;
                } else {
                    std::cout << receivedMessage << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}




static void send_(TCPSocket& socket) {
    try {
        while (true) {
            char str[MAX_INPUT_LENGTH];
            std::cin.getline(str, MAX_INPUT_LENGTH);

            if (std::cin.fail()) {
                std::cerr << "Failed to read input from stdin." << std::endl;
                break;
            }

            std::string message(str);

            if (message == "<exit>")
                exit(0);

            socket.send(message);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}


static void receive_and_send(TCPSocket& socket) {
    std::thread send_t(send_, std::ref(socket));
    std::thread receive_t(receive, std::ref(socket));
    receive_t.join();
    send_t.join();
}

static void connect_to_server(TCPSocket& socket) {
    std::cout << "Connecting to server...\n";

    int attempts = 0;

    while (attempts < MAX_CONNECTION_ATTEMPTS) {
        try {
            socket.connect(SERVER_IP, SERVER_PORT);
            std::cout << "Successfully connected to the server.\n";
            break;
        } catch (const std::exception& e) {
            std::cout << "Connection attempt failed: " << e.what() << std::endl;
            std::cout << "Retrying attempt to connect... (Attempt " << attempts + 1 << ")\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS));
            attempts++;
        }
    }

    if (attempts == MAX_CONNECTION_ATTEMPTS) {
        std::cout << "Maximum connection attempts reached. Unable to connect to the server.\n";
        exit(-1);
    }

    system("cls");
}


int main() {

    setlocale(LC_ALL, "ru");

    TCPSocket::initializeWinsock();

    // Создаем экземпляр TCPSocket
    TCPSocket socket;

    // Запускаем потоки
    std::thread connect_to_server_t(connect_to_server, std::ref(socket));
    connect_to_server_t.join();

    std::thread requests_to_server_t(receive_and_send, std::ref(socket));
    requests_to_server_t.join();

    TCPSocket::cleanup();

    return 0;
}
