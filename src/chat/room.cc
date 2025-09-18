#include "chat/room.hh"

using namespace chat;

void Room::join(Participant participant) {
    _participants.insert(participant);

    for (const auto& msg : _recentMessages) {
        participant->deliver(msg);
    }
}

void Room::leave(Participant participant) {
    _participants.erase(participant);
}

void Room::deliver(const Message& msg) {
    _recentMessages.push_back(msg);

    while(_recentMessages.size() > MAX_RECENT_MSGS) {
        _recentMessages.pop_front();
    }

    for (const auto& participant : _participants) {
        participant->deliver(msg);
    }
}