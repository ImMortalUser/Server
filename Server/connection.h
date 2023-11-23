#pragma once
#include <tcpsocket.h>
#include <thread>
#include <clientrequesthandler.h>
#include <map>

const unsigned short SERVER_PORT = 1414;
const std::string SERVER_IP = "5.187.83.11";


class start_connection_handling
{
public:

    class Connection
    {
    public:


        Connection(TCPSocket& serverSocket, std::map<std::string, int>& cList)
        {
            int clientSocket = serverSocket.acceptConnection();
            std::thread thread(&Connection::handle_connection, std::ref(clientSocket), std::ref(cList));
            Connection nextConnection(serverSocket, cList);
            thread.detach();
        }


        static void handle_connection(int clientSocket, std::map<std::string, int>& cList)
        {
            bool logged = false;
            std::cout << "User connected\n";
            std::string message = "[1] Register\n"
                                  "[2] Log in\n"
                                  "[3] Send message to contact\n"
                                  "[4] Get all messages from contact\n"
                                  "[5] Start chatting with contact\n";
            std::string username = "";
            std::string ip = TCPSocket::getClientIP(clientSocket);

            while (true)
            {

                TCPSocket::sendToClient(clientSocket, message);
                std::string answer = TCPSocket::receiveFromClient(clientSocket);
                if (answer == "")
                    break;
                std::string s = clientrequesthandler::handle_request(answer, ip, clientSocket, username, logged,  cList);
                std::cout << "Currently online:\n";
                for (const auto& [username, socket] : cList)
                    std::cout << username << std::endl;
            }

            if (!username.empty())
            {
                std::cout << "User "<< username <<" disconnected\n";
                database::ToggleOnlineStatus(username, 0);
                cList.erase(username);
            }
            else
            {
                std::cout << "User disconnected\n";
            }
        }

    };

    static void exec()
    {
        TCPSocket socket;
        socket.bindAndListen(SERVER_PORT);
        std::map<std::string, int> clientList;
        Connection connection(socket, clientList);
    }

};
