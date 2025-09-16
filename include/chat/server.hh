#pragma once

#include "util/type.hh"
#include "chat/room.hh"
#include "chat/session.hh"

#include <iostream>
#include <memory>

namespace chat {
    class Server {
    public:
        Server(
            IOContext& ioContext, 
            const TcpEndpoint& endpoint
        );

    private:
        void accept();

        TcpAcceptor _acceptor;
        Room _room;
    };
}