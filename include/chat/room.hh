#pragma once

#include "chat/message.hh"

#include <deque>
#include <set>
#include <memory>

namespace chat {
    class ParticipantImpl {
    public:
    };

    using Participant = std::shared_ptr<ParticipantImpl>;

    class Room {

    };
}