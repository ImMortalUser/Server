#pragma once
#include <database.h>
#include <regexUtils.h>
#include <tcpsocket.h>
#include <vector>
#include <map>


class clientrequesthandler
{
public:
    static std::string handle_request(std::string answer, std::string ip, int clientSocket, std::string& username, bool& logged, std::map<std::string, int>& clientList)
    {

        std::string request;

        if (answer.empty()) {
            return "-1";
        }

        if (answer == "1")
        {
            request = "<reg>";
        }
        else if (answer == "2")
        {
            request = "<login>";
        }
        else if (answer == "3")
        {
            request = "<message>";
        }
        else if (answer == "4")
        {
            request = "<get_messages>";
        }
        else if (answer == "5")
        {
            request = "<wtcw>";
        }
        else
        {
            request = answer;
        }

        if (request == "")
            return "0";

        //<reg>name="name" password="password";
        if (request.find("<reg>") != std::string::npos)
        {

            TCPSocket::sendToClient(clientSocket, "<clrscr>Enter username:\n");
            std::string name = TCPSocket::receiveFromClient(clientSocket);
            TCPSocket::sendToClient(clientSocket, "<clrscr>Enter password:\n");
            std::string password = TCPSocket::receiveFromClient(clientSocket);
            int exit = database::addUserToTable(name, password, ip);
            if (exit == 0)
            {
                TCPSocket::sendToClient(clientSocket, "<clrscr>User registered successfully\n");
            }
            else
            {
                TCPSocket::sendToClient(clientSocket, "<clrscr>User was not registered\n");
            }
            return "0";
        }
        //<login>name="name" password="password";
        else if (request.find("<login>") != std::string::npos)
        {
            TCPSocket::sendToClient(clientSocket, "<clrscr>Enter username:\n");
            std::string name = TCPSocket::receiveFromClient(clientSocket);
            TCPSocket::sendToClient(clientSocket, "<clrscr>Enter password:\n");
            std::string password = TCPSocket::receiveFromClient(clientSocket);
            if (database::userExists(name, password))
            {
                logged = true;
                database::ToggleOnlineStatus(name, 1);
                username = name;
                TCPSocket::sendToClient(clientSocket, "<clrscr>Log in successfully\n");
                clientList[username] = clientSocket;
                return "0";
            }
            else
            {
                TCPSocket::sendToClient(clientSocket, "Username or password is not correct\n");
            }

            return "0";
        }
        else if (request.find("<message>") != std::string::npos)
        {
            if (!logged)
            {
                TCPSocket::sendToClient(clientSocket, "<clrscr>You are not logged in\n");
                return "0";
            }
            TCPSocket::sendToClient(clientSocket, "<clrscr>Enter contact username:\n");
            std::string receiver = TCPSocket::receiveFromClient(clientSocket);
            if (!database::userExists(receiver))
            {
                TCPSocket::sendToClient(clientSocket, "<clrscr>User does not exist\n");
                return "0";
            }
            TCPSocket::sendToClient(clientSocket, "<clrscr>Enter message:\n");
            std::string message = TCPSocket::receiveFromClient(clientSocket);
            if (message == "")
            {
                TCPSocket::sendToClient(clientSocket, "<clrscr>Message is empty\n");
                return "0";
            }
            database::addMessageToMessages(receiver, username, message);
            TCPSocket::sendToClient(clientSocket, "<clrscr>Successfully sended message to " + receiver + '\n');
        }
        else if (request.find("<get_messages>") != std::string::npos)
        {
            if (!logged)
            {
                TCPSocket::sendToClient(clientSocket, "<clrscr>You are not logged in\n");
                return "0";
            }
            TCPSocket::sendToClient(clientSocket, "<clrscr>Enter contact username:\n");
            std::string sender = TCPSocket::receiveFromClient(clientSocket);
            if (!database::userExists(sender))
            {
                TCPSocket::sendToClient(clientSocket, "<clrscr>User does not exist\n");
                return "0";
            }
            std::vector<std::pair<std::string, std::string>> messages = database::getMessages(sender, username);
            TCPSocket::sendToClient(clientSocket, "<clrscr>");
            for (int i = 0; i < messages.size(); i++)
            {
                TCPSocket::sendToClient(clientSocket, '[' + sender + "] " + messages.at(i).first + " " + messages.at(i).second + '\n');
            }
        }
        //WantToChatWith
        else if (request.find("<wtcw>") != std::string::npos)
        {
            if (!logged)
            {
                TCPSocket::sendToClient(clientSocket, "You are not logged in");

                return "0";
            }
            TCPSocket::sendToClient(clientSocket, "<clrscr>Enter contact username:\n");
            std::string friend_ = TCPSocket::receiveFromClient(clientSocket);
            if (!database::userExists(friend_))
            {
                TCPSocket::sendToClient(clientSocket, "<clrscr>Contact was not found\n");
                return "0";
            }

            TCPSocket::sendToClient(clientSocket, "<clrscr>Contact was found, start chatting");
            if (clientList.count(friend_) == 1)
            {
                int friendSocket = clientList[friend_];
                for (;;)
                {
                    std::string message = TCPSocket::receiveFromClient(clientSocket);
                    if (message == "<exit>")
                    {
                        break;
                    }
                    TCPSocket::sendToClient(friendSocket, username + ": " + message);
                    database::addMessageToMessages(friend_, username, message);
                }
            }
            else
            {
                TCPSocket::sendToClient(clientSocket, "<clrscr>Your contact is not online, your messages would be sended to database\n");
                for (;;)
                {
                    std::string message = TCPSocket::receiveFromClient(clientSocket);
                    if (message == "<exit>")
                    {
                        break;
                    }
                    database::addMessageToMessages(friend_, username, message);
                }
            }



            return "0";
        }
        else
        {
            std::cout << "request_not_recognized(" << request << ")\n";
            return "0";
        }
        return "-1";
    }


};
