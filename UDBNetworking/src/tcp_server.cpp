//
// Created by repyanscky on 19.07.22.
//

#include "UDBNetworking/tcp_server.h"
#include <iostream>

namespace UDB {
    using boost::asio::ip::tcp;
    TCPServer::TCPServer(UDB::IPV ipv, int port) :  _ipVersion(ipv), _port(port),
        _acceptor(_ioContext, tcp::endpoint(_ipVersion == IPV::v4 ? tcp::v4() : tcp::v6(), _port)) {

    }

    int TCPServer::Run() {
        try {
            startAccept();
            _ioContext.run();
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
        return 0;
    }

    void TCPServer::startAccept() {
        // Create a connection

        // asynchronously accept the connection
    }
}