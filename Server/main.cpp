#include <thread>
#include <connection.h>
#include <terminal.h>
#include <tcpsocket.h>



int main()
{
    TCPSocket::initializeNetworking();
    database::createDB();
    database::createTableUsers();
    database::createTableMessages();
    std::thread connection_t(start_connection_handling::exec);
    std::thread terminal_t(terminal);
    terminal_t.join();
    return 0;
}
