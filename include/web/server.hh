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
        HttpServer& route(Method method, const std::string& path, RouteHandler handler);

        // middleware support
        HttpServer& use(Middleware middleware);

        // static file serving
        HttpServer& serveStatic(const std::string& urlPath, const std::string& filePath);
        
        void start();
        void stop();

        const HttpServerConfig& config() const { return _config; }
    private:
        void accept();
        HttpResponse handleRequest(const HttpRequest& request);

        struct Route {
            Method method;
            std::string path;
            RouteHandler handler;
        };

        IOContext& _ioc;
        TcpAcceptor _acceptor;
        HttpServerConfig _config;
    
        std::vector<Route> _routes;
        std::vector<Middleware> _middlewares;
        std::unordered_map<std::string, std::string> _staticDirectories;

        bool _isRunning = false;
        friend class HttpConnection;
    };

    class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
    public:
        HttpConnection(TcpSocket socket, HttpServer& server);
        ~HttpConnection() = default;

        void start();
    private:
        void read();
        void processRequest();
        void write(const HttpResponse& response);

        TcpSocket _socket;
        HttpServer& _server;
        boost::asio::streambuf _buffer;

        HttpRequest _request;
        std::string _responseData;
    };
}