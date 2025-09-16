#pragma once

#include "chat/message.hh"

#include <deque>
#include <set>
#include <memory>

namespace chat {
    class ParticipantImpl {
    public:
        virtual ~ParticipantImpl() = default;
        virtual void deliver(const Message& msg) = 0;
    };

    using Participant = std::shared_ptr<ParticipantImpl>;

    class Room {
    public:
        void join(Participant participant);
        void leave(Participant participant);
        void deliver(const Message& msg);
    
    private:
        static constexpr std::size_t MAX_RECENT_MSGS = 100;
        std::set<Participant> _participants;
        std::deque<Message> _recentMessages;
    };
}