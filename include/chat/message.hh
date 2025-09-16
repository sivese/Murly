#pragma once

#include "util/type.hh"

#include <cstdint>
#include <cstring>
#include <string>

namespace chat {
    class Message {
    public:
        static constexpr std::size_t HEADER_LENGTH = 4;
        static constexpr std::size_t MAX_BODY_LENGTH = 512;

    private:
        char data_[HEADER_LENGTH + MAX_BODY_LENGTH];
        std::size_t body_length_;

    public:
        Message() : body_length_(0) { }

        const char* data() const { return data_; }
        char* data() { return data_; }

        std::size_t length() const { return HEADER_LENGTH + body_length_; }

        const char* body() const { return data_ + HEADER_LENGTH; }
        char* body() { return data_ + HEADER_LENGTH; }

        std::size_t body_length() const { return body_length_; }

        void body_length(std::size_t new_length) {
            body_length_ = body_length_ > MAX_BODY_LENGTH ? MAX_BODY_LENGTH : new_length;
        }

        bool decode_header() {
            char header[HEADER_LENGTH + 1] = "";
            std::strncat(header, data_, HEADER_LENGTH);

            body_length_ = std::atoi(header);

            if(body_length_ > MAX_BODY_LENGTH) {
                body_length_ = 0;
                return false;
            }

            return true;
        }

        void encode_header() {
            char header[HEADER_LENGTH + 1] = "";

            std::sprintf(header, "%4d", static_cast<int>(body_length_));
            std::memcpy(data_, header, HEADER_LENGTH);
        }
    };
}