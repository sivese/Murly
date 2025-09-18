#include "chat/server.hh"
#include "chat/session.hh"
#include "chat/room.hh"

using namespace chat;

Server::Server(
    IOContext& ioContext, 
    const TcpEndpoint& endpoint)
    : _acceptor(ioContext, endpoint) {
    accept();
}

void Server::accept() {
    _acceptor.async_accept(
        [this](std::error_code ec, TcpSocket socket) {
            if(!ec) {
                std::cout<< "New client has connected." << socket.remote_endpoint() << std::endl;

                std::make_shared<Session>(std::move(socket), _room)->start();
            }
        }
    );
}