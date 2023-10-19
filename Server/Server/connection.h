#pragma once
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <future>
#include "regexUtils.h"
#include "clientrequesthandler.h"



using boost::asio::ip::tcp;
const std::string SERVER_PORT = "1414";
const std::string SERVER_IP = "5.187.83.11";


class start_connection_handling
{
public:

	class connection2
	{
	public:


		connection2(tcp::acceptor& acceptor, boost::asio::io_context& io_context) : socket(io_context)
		{
			acceptor.accept(socket);
			std::thread thread(handle_connection, std::ref(socket));
			connection2(acceptor, io_context);
			thread.detach();
		}

		static void handle_connection(tcp::socket& socket)
		{
			std::string message = "Hello User";
			std::cout << "user connected";
			boost::system::error_code ignored_error;
			boost::asio::write(socket, boost::asio::buffer(message), ignored_error);

			std::string username = "";
			bool logged = false;

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
				std::string s = clientrequesthandler::handle_request(buf.data(), socket.remote_endpoint().address().to_string(), socket, username, logged);
			}
			std::cout << "User disconnected\n";
			if (username != "")
			{
				database::ToggleOnlineStatus(username, 0);
			}
		}

		tcp::socket socket;
	
		
	};

	static void exec()
	{
		boost::asio::io_context io_context;
		tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), stoi(SERVER_PORT)));
		connection2 connection(acceptor, io_context);
	}
};
