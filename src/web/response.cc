#include "web/response.hh"

#include <chrono>

using namespace web::http;

HttpResponse::HttpResponse(StatusCode code, const std::string& body) 
    : _statusCode(code), _body(body) {
    setDefaultHeaders();
}

HttpResponse& HttpResponse::status(StatusCode code) {
    _statusCode = code;
    return *this;
}

HttpResponse& HttpResponse::body(const std::string& body) {
    _body = body;
    return *this;
}

HttpResponse& HttpResponse::header(const std::string& name, const std::string& value) {
    _headers[name] = value;
    return *this;
}

HttpResponse& HttpResponse::contentType(const std::string& type) {
    _headers["Content-Type"] = type;
    return *this;
}

HttpResponse& HttpResponse::cookie(
    const std::string& name, const std::string& value, 
    const std::string& path, int maxAge
) {
    std::ostringstream oss;
    oss << name << "=" << value << "; Path=" << path;

    if(maxAge >= 0) {
        oss << "; Max-Age=" << maxAge;
    }

    return header("Set-Cookie", oss.str());
}

std::string HttpResponse::getHeader(const std::string& name) const {
    auto it = _headers.find(name);
    return (it != _headers.end()) ? it->second : "";
}

std::string HttpResponse::toString() const {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << static_cast<int>(_statusCode) << " " << statusCodeToString(_statusCode) << "\r\n";

    auto headers = _headers;
    headers["Content-Length"] = std::to_string(_body.length());

    for(const auto& header : headers) {
        oss << header.first << ": " << header.second << "\r\n";
    }

    oss << "\r\n" << _body;

    return oss.str();
}

void HttpResponse::setDefaultHeaders() {
    _headers["Server"] = "HTTP-Lib/1.0";
    _headers["Date"] = getCurrentTimestamp();
    _headers["Connection"] = "close";
}

std::string HttpResponse::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time_t), "%a, %d %b %Y %H:%M:%S GMT");
    return oss.str();
}

std::string HttpResponse::statusCodeToString(StatusCode code) const {
    switch (code) {
        // 1xx Informational
        case StatusCode::CONTINUE: return "Continue";
        case StatusCode::SWITCHING_PROTOCOLS: return "Switching Protocols";
        
        // 2xx Success
        case StatusCode::OK: return "OK";
        case StatusCode::CREATED: return "Created";
        case StatusCode::ACCEPTED: return "Accepted";
        case StatusCode::NO_CONTENT: return "No Content";
        case StatusCode::RESET_CONTENT: return "Reset Content";
        case StatusCode::PARTIAL_CONTENT: return "Partial Content";
        
        // 3xx Redirection
        case StatusCode::MULTIPLE_CHOICES: return "Multiple Choices";
        case StatusCode::MOVED_PERMANENTLY: return "Moved Permanently";
        case StatusCode::FOUND: return "Found";
        case StatusCode::SEE_OTHER: return "See Other";
        case StatusCode::NOT_MODIFIED: return "Not Modified";
        case StatusCode::TEMPORARY_REDIRECT: return "Temporary Redirect";
        case StatusCode::PERMANENT_REDIRECT: return "Permanent Redirect";
        
        // 4xx Client Error
        case StatusCode::BAD_REQUEST: return "Bad Request";
        case StatusCode::UNAUTHORIZED: return "Unauthorized";
        case StatusCode::PAYMENT_REQUIRED: return "Payment Required";
        case StatusCode::FORBIDDEN: return "Forbidden";
        case StatusCode::NOT_FOUND: return "Not Found";
        case StatusCode::METHOD_NOT_ALLOWED: return "Method Not Allowed";
        case StatusCode::NOT_ACCEPTABLE: return "Not Acceptable";
        case StatusCode::REQUEST_TIMEOUT: return "Request Timeout";
        case StatusCode::CONFLICT: return "Conflict";
        case StatusCode::GONE: return "Gone";
        case StatusCode::LENGTH_REQUIRED: return "Length Required";
        case StatusCode::PAYLOAD_TOO_LARGE: return "Payload Too Large";
        case StatusCode::URI_TOO_LONG: return "URI Too Long";
        case StatusCode::UNSUPPORTED_MEDIA_TYPE: return "Unsupported Media Type";
        case StatusCode::RANGE_NOT_SATISFIABLE: return "Range Not Satisfiable";
        case StatusCode::EXPECTATION_FAILED: return "Expectation Failed";
        case StatusCode::IM_A_TEAPOT: return "I'm a teapot";
        case StatusCode::UNPROCESSABLE_ENTITY: return "Unprocessable Entity";
        case StatusCode::TOO_MANY_REQUESTS: return "Too Many Requests";
        
        // 5xx Server Error
        case StatusCode::INTERNAL_SERVER_ERROR: return "Internal Server Error";
        case StatusCode::NOT_IMPLEMENTED: return "Not Implemented";
        case StatusCode::BAD_GATEWAY: return "Bad Gateway";
        case StatusCode::SERVICE_UNAVAILABLE: return "Service Unavailable";
        case StatusCode::GATEWAY_TIMEOUT: return "Gateway Timeout";
        case StatusCode::HTTP_VERSION_NOT_SUPPORTED: return "HTTP Version Not Supported";
        
        default: return "Unknown";
    }
}