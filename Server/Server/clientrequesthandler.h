#pragma once
#include "database.h"
#include "regexUtils.h"
#include "boost\asio.hpp"
#include <vector>


class clientrequesthandler
{
public:
	static std::string handle_request(std::string s, std::string ip, boost::asio::ip::tcp::socket& socket, std::string& username, bool& logged)
	{
		//<reg>name="name" password="password";
		boost::system::error_code ignored_error;
		if (s.find("<reg>") != std::string::npos)
		{
			std::string name = regexUtils::get_name_from_string(s);
			std::string password = regexUtils::get_password_from_string(s);
			int exit = database::addUserToTable(name, password, ip);
			if (exit == 0)
			{
				boost::asio::write(socket, boost::asio::buffer("User registered successfully"), ignored_error);
			}
			else
			{
				boost::asio::write(socket, boost::asio::buffer("User was not registered"), ignored_error);
			}
			return "0";
		}
		//<login>name="name" password="password";
		else if (s.find("<login>") != std::string::npos)
		{
			std::string name = regexUtils::get_name_from_string(s);
			std::string password = regexUtils::get_password_from_string(s);
			if (database::userExists(name, password))
			{
				logged = true;
				database::ToggleOnlineStatus(name, 1);
				username = name;
				return "Log in successfully";
			}
			else
			{
				boost::asio::write(socket, boost::asio::buffer("Username or password is not correct"), ignored_error);
			}
			
			return "0";
		}
		else if (s.find("<message>") != std::string::npos)
		{
			std::string receiver = s.substr(s.find("<to>") + 4, s.find("</to>") - (s.find("<to>") + 4));
			std::string message = s.substr(9, s.find("</message>") - 9);
			if (!database::userExists(receiver))
			{
				std::cout << receiver;
				boost::asio::write(socket, boost::asio::buffer("Receiver does not exist"), ignored_error);
				return "0";
			}
			if (message == "")
			{
				boost::asio::write(socket, boost::asio::buffer("Message is empty"), ignored_error);
				return "0";
			}
			if (!logged)
			{
				boost::asio::write(socket, boost::asio::buffer("You are not logged in"), ignored_error);
				return "0";
			}
			database::addMessageToMessages(receiver, username, message);
		}
		else if (s.find("<get_messages>") != std::string::npos)
		{
			if (!logged)
			{
				boost::asio::write(socket, boost::asio::buffer("You are not logged in"), ignored_error);
				return "0";
			}
			std::string sender = s.substr(14, s.find("</get_messages>") - 14);
			std::vector<std::pair<std::string, std::string>> messages = database::getMessages(sender, username);
			for (int i = 0; i < messages.size(); i++)
			{
				boost::asio::write(socket, boost::asio::buffer('[' + sender + "] " + messages.at(i).first + " " + messages.at(i).second + '\n'), ignored_error);
			}
		}
		else if (s.find("<rt_messenger>") != std::string::npos)
		{
			if (!logged)
			{
				std::cout << "here";
				boost::asio::write(socket, boost::asio::buffer("You are not logged in"), ignored_error);
				return "0";
			}
			std::string companion = s.substr(14, s.find("</rt_messenger>") - 14);
			if (!database::userExists(companion))
			{
				boost::asio::write(socket, boost::asio::buffer("Friend does not exist"), ignored_error);
			}
			if (database::userOnline(companion))
			{
				bool rt = true;
				std::thread send(sendNewMessages, std::ref(socket), std::ref(rt), std::ref(companion), std::ref(username));
				for (;;)
				{
					boost::array<char, 128> buf;
					boost::system::error_code error;
					size_t len = socket.read_some(boost::asio::buffer(buf), error);
					if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset)
					{
						break; // Connection closed cleanly by peer.
					}
					else if (error) {
						throw boost::system::system_error(error); // Some other error.
					}
					std::string message = buf.data();
					database::addMessageToMessages(companion, username, message);
				}
			}
			else
			{
				boost::asio::write(socket, boost::asio::buffer("Friend is not online"), ignored_error);
			}
			
		}
		else
		{
			std::cout << s;
			return "0";
		}
		return "-1";
	}

	static void sendNewMessages(boost::asio::ip::tcp::socket& socket, bool& rt, std::string& sender, std::string& receiver)
	{
		std::vector<std::pair<std::string, std::string>> messages;
		boost::system::error_code ignored_error;
		for (;;)
		{
			if (!rt)
				break;
			std::vector<std::pair<std::string, std::string>> newMessages = database::getMessages(sender, receiver);
			if (messages.size() != newMessages.size())
			{
				int newMes = newMessages.size() - messages.size();
				for (int i = newMessages.size() - 1 - newMes; i < newMessages.size(); i++)
				{
					boost::asio::write(socket, boost::asio::buffer(newMessages.at(i).first + " " + newMessages.at(i).second), ignored_error);
				}
				messages = newMessages;
			}
		}
	}
};