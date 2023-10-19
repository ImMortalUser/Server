#pragma once
#include "boost/asio.hpp"
#include "database.h"
class rtchatting
{
public:
	boost::asio::ip::tcp::socket& socket_my;

	rtchatting(boost::asio::ip::tcp::socket& sm) :
		socket_my(sm)
	{
		std::vector<std::pair<std::string, std::string>> messages;
	}


	
};