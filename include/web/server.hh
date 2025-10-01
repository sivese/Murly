#pragma once

#include "util/type.hh"
#include "web/request.hh"
#include "web/response.hh"

namespace web::http {
    class HttpConnection;

    using RouteHandler  = std::function<HttpResponse(const HttpRequest&)>;
    using Middleware    = std::function<void(HttpRequest&, HttpResponse&, std::function<void()>)>;

    struct HttpServerConfig {
        const int VERSION_MAJOR;
        const int VERSION_MINOR;
        const int VERSION_PATCH;

        inline std::string version() {
            return std::to_string(VERSION_MAJOR) + "." +
                   std::to_string(VERSION_MINOR) + "." +
                   std::to_string(VERSION_PATCH);
        }

        const std::string host;
        std::uint16_t port;
    };

    class HttpServer {
    public:
        HttpServer(IOContext& ioc, const HttpServerConfig& config);
        ~HttpServer() = default;

        HttpServer& get(const std::string& path, RouteHandler handler);
        HttpServer& post(const std::string& path, RouteHandler handler);
        HttpServer& put(const std::string& path, RouteHandler handler);
        HttpServer& del(const std::string& path, RouteHandler handler);
        HttpServer& route(const std::string& method, const std::string& path, RouteHandler handler);
    private:
    };
}