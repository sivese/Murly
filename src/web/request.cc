#include "web/request.hh"

#include <sstream>
#include <algorithm>
#include <cctype>

using namespace web::http;

HttpRequest::HttpRequest(const std::string& rawRequest) { parse(rawRequest); }

bool HttpRequest::parse(const std::string& buffer) {
    reset();

    std::istringstream stream(buffer);
    std::ostringstream bodyStream;
    std::string line;

    bool isFirstLine = true;
    bool headersDone = false;

    while(std::getline(stream, line)) {
        if(line.empty() || line == "\r") {
            line.pop_back();
        }

        if(isFirstLine) {
            parseRequestLine(line);
            isFirstLine = false;
        } 
        else if(!headersDone) {
            if(line.empty()) { headersDone = true; }
            else { parseHeaderLine(line); }
        } 
        else {
            if(bodyStream.tellp() > 0) { bodyStream << "\n"; }
            bodyStream << line;
        }
    }

    _body = bodyStream.str();
    _isComplete = (_method != Method::UNKNOWN && _version != Version::UNKNOWN);

    return _isComplete;
}

bool HttpRequest::isComplete() const {
    if (!_isComplete) { return false; }

    auto contentLengthHeader = getHeader("Content-Length");

    if(contentLengthHeader.has_value()) {
        try {
            auto expectedLength = std::stoull(contentLengthHeader.value());
            return _body.size() >= expectedLength;
        }
        catch(...) {
            return false;
        }
    }

    return true;
}

std::optional<std::string> HttpRequest::getHeader(const std::string& name) const {
    auto lowerName  = toLowerCase(name);
    auto it         = _headers.find(lowerName);

    return (it != _headers.end()) ? std::make_optional(it->second) : std::nullopt;
}

bool HttpRequest::hasHeader(const std::string& name) const {
    auto lowerName = toLowerCase(name);

    return _headers.find(lowerName) != _headers.end();
}

std::size_t HttpRequest::contentLength() const {
    auto header = getHeader("content-length");

    if(header.has_value()) {
        try {
            return std::stoull(header.value());
        }
        catch(...) {
            return 0;
        }
    }

    return 0;
}

std::optional<std::string> HttpRequest::getQueryParam(const std::string& name) const {
    auto params = queryParams();
    auto it     = params.find(name);

    return (it != params.end()) ? std::make_optional(it->second) : std::nullopt;
}