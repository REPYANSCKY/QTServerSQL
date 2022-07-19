#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
int main(int argc, char *argv[])
{
    try {
        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context,tcp::endpoint(tcp::v4(),1337));

        while(true) {
            std::cout << "Accepting connections on port 1337!\n";

            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::cout << "Client connected! Sending message!\n";

            std::string hello_message = "Hello, beautiful client!\n";
            boost::system::error_code error;

            boost::asio::write(socket, boost::asio::buffer(hello_message), error);
        }
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
