#include <iostream>
#include <string>
#include <thread>
#include "database.h"
#include "connection.h"
#include "terminal.h"



int main()
{
	setlocale(LC_ALL, "ru");
	database::createDB();
	database::createTableUsers();
	database::createTableMessages();
	std::thread connection_t(start_connection_handling::exec);
	std::thread terminal_t(terminal);
	terminal_t.join();
	return 0;
}
