//
// Created by repyanscky on 19.07.22.
//

#ifndef UDBCLIENT_TCP_SERVER_H
#define UDBCLIENT_TCP_SERVER_H

#include <string>
#include <boost/asio.hpp>

namespace UDB {
    enum IPV {
        v4,
        v6
    };
    class TCPServer {
    public:
        TCPServer(IPV ipv, int port);

        int Run();

    private:
        void startAccept();
    private:
        IPV _ipVersion;
        int _port;

        boost::asio::io_context _ioContext;
        boost::asio::ip::tcp::acceptor _acceptor;
    };
}

#endif //UDBCLIENT_TCP_SERVER_H
