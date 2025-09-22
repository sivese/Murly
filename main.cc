#include "app/chat.hh"

auto main(int argc, char* argv[]) -> int {
    app::Chatting chat;

    chat.run();
    
    return 0;
}