#include "chat/server.hh"
#include "chat/session.hh"
#include "chat/room.hh"

using namespace chat;

Server::Server(
    IOContext& ioContext, 
    const TcpEndpoint& endpoint)
    : _acceptor(ioContext, endpoint) {

    // Enable address reuse to avoid "Address already in use" errors
    _acceptor.set_option(TcpAcceptor::reuse_address(true));
    
    std::cout<< "Sever started on " << endpoint << std::endl;

    // Start accepting incoming connections
    // TODO : Set user number limit
    accept();
}

void Server::accept() {
    _acceptor.async_accept(
        [this](std::error_code ec, TcpSocket socket) {
            if(!ec) {
                try {
                    std::cout<< "New client has connected." << socket.remote_endpoint() << std::endl;

                    std::make_shared<Session>(std::move(socket), _room)->start();
                }
                catch(const std::exception& e) {
                    std::cerr<< "Error: " << e.what() << std::endl;
                }
            }
            else {
                std::cerr<< "Accept error: " << ec.message() << std::endl;
            }

            accept();
        }
    );
}