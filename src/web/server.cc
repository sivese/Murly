#include "web/server.hh"
#include "web/utils.hh"

#include <iostream>

using namespace web::http;

HttpServer::HttpServer(IOContext& ioc, const HttpServerConfig& config)
    :   _ioc(ioc), 
        _acceptor(ioc, TcpEndpoint(boost::asio::ip::tcp::v4(), config.port)),
        _config(config) {

    // Enable address reuse
    //_acceptor.set_option(boost::asio::socket_base::reuse_address(true));

    std::cout<<"Server version: "<<_config.version()<<std::endl;
    std::cout<<"Listening on: "<<_config.host<<":"<<_config.port<<std::endl;
}

void HttpServer::start() {
    if(_isRunning) {
        std::cerr<<"Server is already running!"<<std::endl;
        return;
    }

    _isRunning = true;
    std::cout<<"Http server started on port "<<_config.port<<std::endl;
    accept();
}

void HttpServer::stop() {
    if(!_isRunning) return;

    _isRunning = false;
    _acceptor.close();
    std::cout<<"Http server stopped."<<std::endl;
}

void HttpServer::accept() {
    _acceptor.async_accept(
        [this](std::error_code ec, TcpSocket socket) {
            if(!ec) {
                try{
                    std::cout<<"New connection from: "<<socket.remote_endpoint()<<std::endl;
                
                    std::make_shared<HttpConnection>(std::move(socket), *this)->start();
                }
                catch(const std::exception& e) {
                    std::cerr<<"Connection error: "<<e.what()<<std::endl;
                }
            }
            else {
                std::cerr<<"Accept error: "<<ec.message()<<std::endl;
            }

            if(_isRunning) accept();
        }
    );
}

HttpServer& HttpServer::get(const std::string& path, RouteHandler handler) {
    return route(Method::GET, path, handler);
}

HttpServer& HttpServer::post(const std::string& path, RouteHandler handler) {
    return route(Method::POST, path, handler);
}

HttpServer& HttpServer::put(const std::string& path, RouteHandler handler) {
    return route(Method::PUT, path, handler);
}

HttpServer& HttpServer::del(const std::string& path, RouteHandler handler) {
    return route(Method::DELETE, path, handler);
}

HttpServer& HttpServer::route(Method method, const std::string& path, RouteHandler handler) {
    _routes.push_back({method, path, handler});
    std::cout<<"Registered route: "<< static_cast<int>(method) <<" "<<path<<std::endl;
    return *this;
}

HttpServer& HttpServer::use(Middleware middleware) {
    _middlewares.push_back(middleware);
    return *this;
}

HttpServer& HttpServer::serveStatic(const std::string& path, const std::string& directory) {
    _staticDirectories[path] = directory;
    std::cout<<"Serving static files: "<<path<<" -> "<<directory<<std::endl;
    return *this;
}

HttpResponse HttpServer::handleRequest(const HttpRequest& request) {
    
}