#pragma once

#include "util/type.hh"

#include <cstdint>
#include <cstring>
#include <string>

namespace chat {
    struct Message {
    public:
        static constexpr std::size_t HEADER_LENGTH = 4;
        static constexpr std::size_t MAX_BODY_LENGTH = 512;

    private:
        char _data[HEADER_LENGTH + MAX_BODY_LENGTH];
        std::size_t _bodyLength;

    public:
        Message();

        const char* data() const    { return _data; }
        char* data()                { return _data; }

        std::size_t length() const  { return HEADER_LENGTH + _bodyLength; }

        const char* body() const    { return _data + HEADER_LENGTH; }
        char* body()                { return _data + HEADER_LENGTH; }

        std::size_t bodyLength() const { return _bodyLength; }

        void bodyLength(std::size_t newLength);
        bool decodeHeader();
        void encodeHeader();
    };
}