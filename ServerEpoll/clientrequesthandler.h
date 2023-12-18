#pragma once
#include <database.h>
#include <regexUtils.h>
#include <tcpsocket.h>
#include <vector>
#include <map>
#include "regexUtils.h"

class clientrequesthandler
{
public:
    static std::string handle_request(std::string clientAnswer, std::string ip, int clientSocket, std::string& username, bool& logged, std::map<std::string, int>& clientList, std::string& chattingWith)
    {
        //<reg> name password
        if (chattingWith != "")
        {
            if (clientList.count(chattingWith) == 1)
            {
                int friendSocket = clientList[chattingWith];
                std::string message = clientAnswer;
                if (message == "<exit>")
                {
                    chattingWith = "";
                    return "chatting stopped";
                }
                TCPSocket::sendToClient(friendSocket, username + ": " + message);
                database::addMessageToMessages(chattingWith, username, message);
            }
            else
            {
                std::string message = clientAnswer;
                if (message == "<exit>")
                {
                    chattingWith = "";
                    return "chatting stopped";
                }
                database::addMessageToMessages(chattingWith, username, message);
            }
            return " ";
        }
        else if (clientAnswer.find("<reg>") != std::string::npos)
        {
            std::string name = RegexUtils::extractFirstArgFromTwo(clientAnswer);
            std::string password = RegexUtils::extractSecondArgFromTwo(clientAnswer);
            int exit = database::addUserToTable(name, password, ip);
            if (exit == 0)
            {
                return "<clrscr>User registered successfully\n";
            }
            else
            {
                return "<clrscr>Error, user wasn't registered\n";
            }
        }
        //<login> name password ;
        else if (clientAnswer.find("<login>") != std::string::npos)
        {
            std::string name = RegexUtils::extractFirstArgFromTwo(clientAnswer);
            std::string password = RegexUtils::extractSecondArgFromTwo(clientAnswer);
            if (database::userExists(name, password))
            {
                logged = true;
                database::ToggleOnlineStatus(name, 1);
                username = name;
                clientList[username] = clientSocket;
                return database::vectorTupleToString(database::getMessages(username));
            }
            else
            {
                return "Username or password is not correct\n";
            }
        }
        else if (clientAnswer.find("<message>") != std::string::npos)
        {
            if (!logged)
            {
                return "<clrscr>You are not logged in\n";
            }
            std::string receiver = RegexUtils::extractFirstArgFromTwo(clientAnswer);
            if (!database::userExists(receiver))
            {
                return "<clrscr>User does not exist\n";
            }
            std::string message = RegexUtils::extractSecondArgFromTwo(clientAnswer);
            if (message == "")
            {
                return "<clrscr>Message is empty\n";
            }
            database::addMessageToMessages(receiver, username, message);
            return "<clrscr>Successfully sended message to " + receiver + '\n';
        }
        else if (clientAnswer.find("<get_messages>") != std::string::npos)
        {
            if (!logged)
            {
                return "<clrscr>You are not logged in\n";
            }
            std::string sender = RegexUtils::extractArgFromOne(clientAnswer);
            if (!database::userExists(sender))
            {
                return "<clrscr>User does not exist\n";
            }
            std::vector<std::pair<std::string, std::string>> messages = database::getMessages(sender, username);
            std::string ret = "<clrscr>";
            for (int i = 0; i < messages.size(); i++)
            {
                ret += '[' + sender + "] " + messages.at(i).first + " " + messages.at(i).second + '\n';
            }
            return ret;
        }
        //WantToChatWith
        else if (clientAnswer.find("<wtcw>") != std::string::npos)
        {
            if (!logged)
            {
                return "You are not logged in";
            }
            std::string friend_ = RegexUtils::extractArgFromOne(clientAnswer);
            //std::cout << friend_ + " ";
            if (!database::userExists(friend_))
            {
                return "<clrscr>Contact was not found\n";
            }
            chattingWith = friend_;
            return "<clrscr>Contact was found, start chatting\n";
        }
        else if (clientAnswer != "")
        {
            //std::cout << "request_not_recognized(" << clientAnswer << ")\n";
            return "Error: request not recognized";
        }
    }


};
