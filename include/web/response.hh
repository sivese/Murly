#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <sstream>
#include <ctime>

namespace web::http {
    enum class StatusCode {
        // 1xx Informational
        CONTINUE            = 100,
        SWITCHING_PROTOCOLS = 101,
        // 2xx Success
        OK                   = 200,
        CREATED              = 201,
        ACCEPTED             = 202,
        NO_CONTENT          = 204,
        RESET_CONTENT       = 205,
        PARTIAL_CONTENT     = 206,
        // 3xx Redirection
        MULTIPLE_CHOICES    = 300,
        MOVED_PERMANENTLY   = 301,
        FOUND               = 302,
        SEE_OTHER           = 303,
        NOT_MODIFIED        = 304,
        TEMPORARY_REDIRECT  = 307,
        PERMANENT_REDIRECT  = 308,
        // 4xx Client Error
        BAD_REQUEST          = 400,
        UNAUTHORIZED         = 401,
        PAYMENT_REQUIRED     = 402,
        FORBIDDEN            = 403,
        NOT_FOUND            = 404,
        METHOD_NOT_ALLOWED   = 405,
        NOT_ACCEPTABLE       = 406,
        REQUEST_TIMEOUT      = 408,
        CONFLICT             = 409,
        GONE                 = 410,
        LENGTH_REQUIRED     = 411,
        PAYLOAD_TOO_LARGE   = 413,
        URI_TOO_LONG        = 414,
        UNSUPPORTED_MEDIA_TYPE = 415,
        RANGE_NOT_SATISFIABLE = 416,
        EXPECTATION_FAILED  = 417,
        IM_A_TEAPOT        = 418,
        UNPROCESSABLE_ENTITY = 422,
        TOO_MANY_REQUESTS   = 429,
        // 5xx Server Error
        INTERNAL_SERVER_ERROR= 500,
        NOT_IMPLEMENTED      = 501,
        BAD_GATEWAY          = 502,
        SERVICE_UNAVAILABLE  = 503,
        GATEWAY_TIMEOUT      = 504,
        HTTP_VERSION_NOT_SUPPORTED = 505
    };

    class HttpResponse {
    public:
        HttpResponse() = default;
        explicit HttpResponse(StatusCode code, const std::string& body = "");

        void setDefaultHeaders();
        
        std::string statusCodeToString(StatusCode code) const;
        std::string getCurrentTimestamp() const;

        HttpResponse& status(StatusCode code);
        HttpResponse& body(const std::string& body);
        HttpResponse& header(const std::string& name, const std::string& value);
        HttpResponse& contentType(const std::string& type);
        HttpResponse& cookie(
            const std::string& name, const std::string& value, 
            const std::string& path = "/", int maxAge = -1
        );

        std::string getHeader(const std::string& name) const;
        std::string toString() const;

        //Helper methods for common responses
        static HttpResponse ok(const std::string& body) {
            return HttpResponse(StatusCode::OK, body);
        }

        static HttpResponse created(const std::string& body) {
            return HttpResponse(StatusCode::CREATED, body);
        }

        static HttpResponse noContent() {
            return HttpResponse(StatusCode::NO_CONTENT);
        }

        static HttpResponse badRequest(const std::string& message) {
            return HttpResponse(StatusCode::BAD_REQUEST, message);
        }

        static HttpResponse unauthorized(const std::string& message) {
            return HttpResponse(StatusCode::UNAUTHORIZED, message);
        }

        static HttpResponse forbidden(const std::string& message) {
            return HttpResponse(StatusCode::FORBIDDEN, message);
        }

        static HttpResponse notFound(const std::string& message) {
            return HttpResponse(StatusCode::NOT_FOUND, message);
        }

        static HttpResponse methodNotAllowed(const std::string& message) {
            return HttpResponse(StatusCode::METHOD_NOT_ALLOWED, message);
        }

        static HttpResponse internalError(const std::string& message) {
            return HttpResponse(StatusCode::INTERNAL_SERVER_ERROR, message);
        }

        static HttpResponse notImplemented(const std::string& message) {
            return HttpResponse(StatusCode::NOT_IMPLEMENTED, message);
        }

        static HttpResponse serviceUnavailable(const std::string& message) {
            return HttpResponse(StatusCode::SERVICE_UNAVAILABLE, message);
        }


        // Content type helpers
        static HttpResponse json(const std::string& json, StatusCode status) {
            return HttpResponse(status, json).contentType("application/json");
        }

        static HttpResponse html(const std::string& html, StatusCode status) {
            return HttpResponse(status, html).contentType("text/html; charset=utf-8");
        }

        static HttpResponse text(const std::string& text, StatusCode status) {
            return HttpResponse(status, text).contentType("text/plain; charset=utf-8");
        }

        static HttpResponse redirect(const std::string& location, StatusCode status) {
            return HttpResponse(status).header("Location", location);
        }
    private:
        StatusCode _statusCode;
        std::string _body;
        std::unordered_map<std::string, std::string> _headers;
    };
}