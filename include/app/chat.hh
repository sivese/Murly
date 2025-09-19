#pragma once

#include <memory>
#include <thread>

namespace app {
    class Chatting {
    public:
        Chatting() = default;
        ~Chatting() = default;

        void run();
    private:
        void createConnection();
        void openChatServer();

        std::unique_ptr<std::thread> _serverThread;
    };
}