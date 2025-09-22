#include "chat/session.hh"

#include <iostream>

using namespace chat;

Session::Session(
    TcpSocket socket,
    Room& room
) : _socket(std::move(socket)),
    _room(room)
{
    std::cout<<"A session has been created."<<std::endl;
}

void Session::start() {
    _room.join(shared_from_this()); //what is this?
    readHeader();
}

void Session::deliver(const Message& msg) {
    bool writeInProgress = !_writeMsgs.empty();

    _writeMsgs.push_back(msg);
    
    if (!writeInProgress) {
        write();
    }
}

void Session::readHeader() {
    auto self(shared_from_this());

    boost::asio::async_read(
        _socket,
        boost::asio::buffer(_readMsg.data(), Message::HEADER_LENGTH),
        [this, self](std::error_code ec, std::size_t /*length*/) {
            if (!ec && _readMsg.decodeHeader()) {
                readBody();
            } 
            else {
                std::cout<<" Disconnect from client. An error occurred on reading header: "<<ec.message()<<std::endl;
                _room.leave(shared_from_this());
            }
        }
    );
}

void Session::readBody() {
    auto self(shared_from_this());

    boost::asio::async_read(
        _socket,
        boost::asio::buffer(_readMsg.body(), _readMsg.bodyLength()),
        [this, self](std::error_code ec, std::size_t /*length*/) {
            if(!ec) {
                _room.deliver(_readMsg);
                readHeader();
            } 
            else {
                std::cout<<" Disconnect from client. An error occurred on reading body: "<<ec.message()<<std::endl;
                _room.leave(shared_from_this());
            }
        }
    );
}

void Session::write() {
    auto self(shared_from_this());

    boost::asio::async_write(
        _socket,
        boost::asio::buffer(
            _writeMsgs.front().data(), 
            _writeMsgs.front().length()
        ),
        [this, self](std::error_code ec, std::size_t /*length*/) {
            if(!ec) {
                _writeMsgs.pop_front();

                if(!_writeMsgs.empty()) {
                    write();
                }
            }
            else {
                std::cout<<" Disconnect from client. An error occurred on writing : "<<ec.message()<<std::endl;
                _room.leave(shared_from_this());
            }
        }
    );
}