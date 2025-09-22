#include "chat/message.hh"

using namespace chat;

Message::Message() : _bodyLength(0) { }

void Message::bodyLength(std::size_t newLength) {
    _bodyLength = newLength > MAX_BODY_LENGTH ? MAX_BODY_LENGTH : newLength;
}

bool Message::decodeHeader() {
    char header[HEADER_LENGTH + 1] = "";
    std::strncat(header, _data, HEADER_LENGTH);

    _bodyLength = std::atoi(header);

    if(_bodyLength > MAX_BODY_LENGTH) {
        _bodyLength = 0;
        return false;
    }

    return true;
}

void Message::encodeHeader() {
    char header[HEADER_LENGTH + 1] = "";

    std::sprintf(header, "%4d", static_cast<int>(_bodyLength));
    std::memcpy(_data, header, HEADER_LENGTH);
}