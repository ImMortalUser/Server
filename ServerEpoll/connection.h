#pragma once
#include <tcpsocket.h>
#include <thread>
#include <clientrequesthandler.h>
#include <map>
#include <vector>
#include <sys/epoll.h>

const unsigned short SERVER_PORT = 1414;
const std::string SERVER_IP = "5.187.83.11";
static std::string REQUEST_NEW_DATA_MESSAGE = "<GET_NEW_DATA>";

class start_connection_handling {
public:
    class Connection {
    public:
        Connection(TCPSocket& serverSocket, std::map<std::string, int>& cList, int epoll_fd) {
            int clientSocket = serverSocket.acceptConnection();
            struct epoll_event event;
            event.events = EPOLLIN | EPOLLRDHUP;
            event.data.fd = clientSocket;
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientSocket, &event);

            std::thread thread(&Connection::handle_connection, clientSocket, std::ref(cList), epoll_fd);
            thread.detach();
        }

        static void handle_connection(int clientSocket, std::map<std::string, int>& cList, int epoll_fd) {
            bool logged = false;
            std::string username = "";
            std::string ip = TCPSocket::getClientIP(clientSocket);
            std::string chattingWith = "";
            //std::cout << "Пользователь подключен\n";
            std::string clientAnswer;

            while (true) {
                struct epoll_event events;
                epoll_wait(epoll_fd, &events, 1, -1);

                if (events.events & (EPOLLIN | EPOLLRDHUP)) {
                    clientAnswer = TCPSocket::receiveFromClient(clientSocket);
                    if (clientAnswer == "")
                        break;
                    std::string serverAnswer = clientrequesthandler::handle_request(clientAnswer, ip, clientSocket, username, logged,  cList, chattingWith);
                    TCPSocket::sendToClient(clientSocket, serverAnswer);
                }
            }

//            if (!username.empty()) {
//                std::cout << "Пользователь " << username << " отключен\n";
//                database::ToggleOnlineStatus(username, 0);
//                cList.erase(username);
//            } else {
//                std::cout << "Пользователь отключен\n";
//            }
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientSocket, nullptr);
            close(clientSocket);
        }
    };

    static void exec() {
        TCPSocket socket;
        socket.bindAndListen(SERVER_PORT);

        int epoll_fd = epoll_create1(0);
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = socket.getSocketFD();
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket.getSocketFD(), &event);

        std::map<std::string, int> clientList;
        std::vector<Connection> connections;

        while (true) {
            struct epoll_event events[10];
            int numEvents = epoll_wait(epoll_fd, events, 10, -1);

            for (int i = 0; i < numEvents; ++i) {
                if (events[i].data.fd == socket.getSocketFD()) {
                    connections.emplace_back(socket, clientList, epoll_fd);
                }
            }
        }
    }
};
