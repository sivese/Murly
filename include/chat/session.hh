#pragma once

#include "util/type.hh"
#include "chat/message.hh"
#include "chat/room.hh"

#include <deque>
#include <memory>

namespace chat {
    class Session 
        : public ParticipantImpl, 
        public std::enable_shared_from_this<Session> {
    public:
        Session(TcpSocket socket, Room& room);
        void start();
        void deliver(const Message& msg) override;

    private:
        void readHeader();
        void readBody();
        void write();

        TcpSocket   _socket;
        Room&       _room;
        Message     _readMsg;

        std::deque<Message> _writeMsgs;
    };
}