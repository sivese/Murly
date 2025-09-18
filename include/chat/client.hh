#pragma once

#include "chat/message.hh"
#include "util/type.hh"

#include <deque>
#include <iostream>
#include <thread>

namespace chat {
    class Client {
    public:
        Client(
            IOContext& ioContext, 
            const TcpResolver::results_type& endpoints
        );

        void write(const Message& msg);
        void write();
        void close();

    private:
        void connect(const TcpResolver::results_type& endpoints);
        void readHeader();
        void readBody();
        void doWrite();

    private:
        IOContext& _ioContext;
        TcpSocket _socket;
        Message _readMsg;
        std::deque<Message> _writeMsgs;
    };
}