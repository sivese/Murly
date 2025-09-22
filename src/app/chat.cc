#include "app/chat.hh"

#include "chat/server.hh"
#include "chat/client.hh"

#include "util/type.hh"

#include <iostream>
#include <thread>

using namespace app;

void Chatting::run() {
    while(true) {
        std::cout << "Please select an option:\n";
        std::cout << "1. start a chat server\n";
        std::cout << "2. create a connection with a chat server\n";
        std::cout << "3. exit\n";

        auto choice = 2;
        //std::cin >> choice;

        switch(choice) {
            case 1:
                openChatServer();
                break;
            case 2:
                createConnection();
                break;
            case 3:
                std::cout << "Exiting the chat application.\n";
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

void Chatting::createConnection() {
    try {
        auto ioContext = IOContext();
        auto resolver  = TcpResolver(ioContext);

        auto endpoints  = resolver.resolve("localhost", std::to_string(8080));
        auto client     = chat::Client(ioContext, endpoints);

        auto ioThread = std::thread([&ioContext]() { ioContext.run(); });

        auto line = new char[chat::Message::MAX_BODY_LENGTH + 1];

        std::cout << "Connected to chat server. You can start sending messages.\n";
        std::cout << "Type your messages below (Press Ctrl+D to exit):\n";

        while(std::cin.getline(line, chat::Message::MAX_BODY_LENGTH + 1)) {
            auto msg = chat::Message();
            msg.bodyLength(std::strlen(line));

            std::memcpy(msg.body(), line, msg.bodyLength());
            msg.encodeHeader();

            client.write(msg);
        }

        client.close();
        ioThread.join();

        delete[] line;
    }
    catch (const std::exception& e) {
        std::cerr << "Error connecting to chat server: " << e.what() << "\n";
    }
}

void Chatting::openChatServer() {
    auto port = 0;

    std::cout << "Enter port number to start the chat server (default: 8080): ";
    std::cin >> port;

    if (port <= 0) { port = 8080; } // Default port

    std::cout << "Starting chat server on port " << port << "...\n";

    try {
        auto ioContext  = IOContext();
        
        auto endpoint = TcpEndpoint(boost::asio::ip::tcp::v4(), port);
        auto server = chat::Server(ioContext, endpoint);

        std::cout << "Chat server has been started at port " << port << " successfully.\n";
    
        ioContext.run();
    } 
    catch (const std::exception& e) {
        std::cerr << "Error starting chat server: " << e.what() << "\n";
    }
}
