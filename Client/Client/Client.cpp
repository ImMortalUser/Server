#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <future>
#include <boost/array.hpp>
#include <time.h>
#include <chrono>

const std::string SERVER_PORT = "1414";
const std::string SERVER_IP = "5.187.83.11";
boost::asio::io_context io_context;
boost::asio::ip::tcp::socket socket_(io_context);
char str[256];

static void receive()
{
    try
    {
        for (;;)
        {
            boost::system::error_code error;
            boost::array<char, 128> buf;
            size_t len = socket_.read_some(boost::asio::buffer(buf), error);
            if (len > 0)
            {
                std::cout << "[Server] ";
                std::cout.write(buf.data(), len);
                std::cout << std::endl;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

static void send_()
{
    try
    {
        boost::system::error_code ignored_error;
        for (;;)
        {
            std::cin.getline(str, 256);
            boost::asio::write(socket_, boost::asio::buffer(str), ignored_error);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
    }
}

static void receive_and_send()
{
    std::thread send_t(send_);
    std::thread receive_t(receive);
    receive_t.join();
    send_t.join();
}
//<message>TEST MESSAGE</message><to>c</to>
//<login>name:b password:b
//<wtcw><friend>c</friend>
static void connect_to_server()
{
    std::cout << "Connecting to server...\n";
    using boost::asio::ip::tcp;
    
        for (;;)
        {
            try
            {

                tcp::resolver resolver(io_context);
                tcp::resolver::results_type endpoints =
                    resolver.resolve(SERVER_IP, SERVER_PORT);
                boost::asio::connect(socket_, endpoints);
                std::cout << "Connected successfully\n";
                break;
            }
            catch (const std::exception& e)
            {
                std::cout << "Connection failed: ";
                std::cerr << e.what() << std::endl;
                std::cout << "Retrying attempt to connect...\n";
            }
        }
        
}

int main()
{
    setlocale(LC_ALL, "ru");
    bool isConnected_a = false;
    bool isConnected_r = false;
    std::thread connect_to_server_t(connect_to_server);
    connect_to_server_t.join();
    std::thread requests_to_server_t(receive_and_send);
    requests_to_server_t.join();
}
