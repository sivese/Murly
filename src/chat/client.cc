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
                write();
            }
        }
    );
}

void Client::close() {
    boost::asio::post(
        _ioContext,
        [this]() { 
            if(_socket.is_open()) {
                std::cout << "Closing connection..." << std::endl;
                _socket.close(); 
            }
        }
    );
}

void Client::connect(const TcpResolver::results_type& endpoints) {
    boost::asio::async_connect(
        _socket, endpoints,
        [this](std::error_code ec, TcpEndpoint ep) {
            if(!ec) {
                std::cout<< "Connected to server at " << ep << std::endl;
                readHeader();
            }
            else {
                std::cerr<< "Connection failed, error: " << ec.message() << std::endl;
            }
        }
    );
}

void Client::readHeader() {
    boost::asio::async_read(
        _socket,
        boost::asio::buffer(
            _readMsg.data(), 
            Message::HEADER_LENGTH
        ),
        [this](std::error_code ec, std::size_t /*length*/) {
            if (!ec && _readMsg.decodeHeader()) {
                readBody();
            } 
            else {
                std::cerr<< "Failed to read header: " << ec.message() << std::endl;
                _socket.close();
            }
        }
    );
}

void Client::readBody() {
    boost::asio::async_read(
        _socket,
        boost::asio::buffer(
            _readMsg.body(), 
            _readMsg.bodyLength()
        ),
        [this](std::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                std::cout<<">> ";
                std::cout.write(_readMsg.body(), _readMsg.bodyLength());
                std::cout<<"\n";

                readHeader();
            } 
            else {
                std::cerr<< "Failed to read body: " << ec.message() << std::endl;
                _socket.close();
            }
        }
    );
}

void Client::write() {
    boost::asio::async_write(
        _socket,
        boost::asio::buffer(
            _writeMsgs.front().data(),
            _writeMsgs.front().length()
        ),
        [this](std::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                _writeMsgs.pop_front();

                if(!_writeMsgs.empty()) write();
            }
            else {
                _socket.close();
            }
        }
    );
}