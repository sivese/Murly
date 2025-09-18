#include "chat/message.hh"
#include "chat/client.hh"

using namespace chat;

Client::Client(
    IOContext& ioContext, 
    const TcpResolver::results_type& endpoints)
    : _ioContext(ioContext), _socket(ioContext) {
    connect(endpoints);
}

void Client::write(const Message& msg) {
    boost::asio::post(
        _ioContext,
        [this, msg]() {
            bool writeInProgress = !_writeMsgs.empty();

            _writeMsgs.push_back(msg);

            if (!writeInProgress) {
                doWrite();
            }
        }
    );
}

void Client::close() {
    boost::asio::post(
        _ioContext,
        [this]() { _socket.close(); }
    );
}

void Client::connect(const TcpResolver::results_type& endpoints) {
    boost::asio::async_connect(
        _socket, endpoints,
        [this](std::error_code ec, TcpEndpoint) {
            if(!ec) readHeader();
        }
    );
}

void Client::readHeader() {
    boost::asio::async_read(
        _socket,
        boost::asio::buffer(_readMsg.data(), Message::HEADER_LENGTH),
        [this](std::error_code ec, std::size_t /*length*/) {
            if (!ec && _readMsg.decodeHeader()) {
                readBody();
            } else {
                _socket.close();
            }
        }
    );
}

void Client::readBody() {

}

void Client::write() {
    
}