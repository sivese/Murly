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
    auto params = getQueryParams();
    auto it     = params.find(name);

    return (it != params.end()) ? std::make_optional(it->second) : std::nullopt;
}

std::unordered_map<std::string, std::string> HttpRequest::getQueryParams() const {
    std::unordered_map<std::string, std::string> params;
    //parseQuery(_query);

    std::istringstream queryStream(_query);
    std::string pair;

    while(std::getline(queryStream, pair, '&')) {
        std::size_t equalPos = pair.find('=');

        if(equalPos != std::string::npos) {
            auto key = pair.substr(0, equalPos);
            auto value = pair.substr(equalPos + 1);
            params[key] = value;
        }
        else {
            params[pair] = "";
        }
    }

    return params;
}

std::string HttpRequest::methodToString() const {
    switch (_method) {
        case Method::GET:       return "GET";
        case Method::POST:      return "POST";
        case Method::PUT:       return "PUT";
        case Method::DELETE:    return "DELETE";
        case Method::HEAD:      return "HEAD";
        case Method::OPTIONS:   return "OPTIONS";
        case Method::PATCH:     return "PATCH";
        case Method::CONNECT:   return "CONNECT";
        case Method::TRACE:     return "TRACE";
        
        default:                return "UNKNOWN";
    }
}

std::string HttpRequest::versionToString() const {
    switch (_version) {
        case Version::HTTP_1_0: return "HTTP/1.0";
        case Version::HTTP_1_1: return "HTTP/1.1";

        //No support for HTTP/2 and HTTP/3 right now
        //Maybe in the future
        case Version::HTTP_2_0: return "HTTP/2.0";
        case Version::HTTP_3_0: return "HTTP/3.0";

        default: return "UNKNOWN";
    }
}

std::string HttpRequest::toString() const {
    std::ostringstream oss;
    oss << methodToString() << " " << _uri << " " << versionToString() << "\r\n";

    for(const auto& header : _headers) {
        oss << header.first << ": " << header.second << "\r\n";
    }

    oss << "\r\n" << _body;
    return oss.str();
}

void HttpRequest::reset() {
    _method     = Method::UNKNOWN;
    _uri.clear();
    _path.clear();
    _query.clear();
    _version    = Version::UNKNOWN;
    _headers.clear();
    _body.clear();
    _isComplete = false;
}

Method HttpRequest::stringToMethod(const std::string& method) const {
    std::string method_ = toUpperCase(method);

    if (method_ == "GET")       return Method::GET;
    if (method_ == "POST")      return Method::POST;
    if (method_ == "PUT")       return Method::PUT;
    if (method_ == "DELETE")    return Method::DELETE;
    if (method_ == "HEAD")      return Method::HEAD;
    if (method_ == "OPTIONS")   return Method::OPTIONS;
    if (method_ == "PATCH")     return Method::PATCH;
    if (method_ == "CONNECT")   return Method::CONNECT;
    if (method_ == "TRACE")     return Method::TRACE;

    return Method::UNKNOWN;
}

Version HttpRequest::stringToVersion(const std::string& version) const {
    std::string version_ = toUpperCase(version);    

    if (version_ == "HTTP/1.0") return Version::HTTP_1_0;
    if (version_ == "HTTP/1.1") return Version::HTTP_1_1;

    //As I mentioned before, no support for HTTP/2 and HTTP/3 right now
    //So this is just for future reference
    if (version_ == "HTTP/2.0") return Version::HTTP_2_0;
    if (version_ == "HTTP/3.0") return Version::HTTP_3_0;

    return Version::UNKNOWN;
}

std::string HttpRequest::toLowerCase(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);

    return result;
}

std::string HttpRequest::toUpperCase(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);

    return result;
}

void HttpRequest::parseRequestLine(const std::string& line) {
    std::istringstream iss(line);
    std::string methodStr, versionStr;

    iss >> methodStr >> _uri >> versionStr;

    _method  = stringToMethod(methodStr);
    _version = stringToVersion(versionStr);

    parseUri(_uri);
}

void HttpRequest::parseHeaderLine(const std::string& line) {
    auto colonPos = line.find(':');

    if(colonPos != std::string::npos) {
        auto name   = line.substr(0, colonPos);
        auto value  = line.substr(colonPos + 1);

        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t\r") + 1);

        _headers[toLowerCase(name)] = value;
    }
}

void HttpRequest::parseUri(const std::string& uri) {
    auto queryPos = uri.find('?');

    if(queryPos != std::string::npos) {
        _path = uri.substr(0, queryPos);
        _query = uri.substr(queryPos + 1);
    } 
    else {
        _path = uri;
        _query.clear();
    }
}

void HttpRequest::parseQuery(const std::string& query) {

}