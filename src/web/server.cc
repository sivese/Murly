#include "web/server.hh"
#include "web/utils.hh"

#include <iostream>
#include <filesystem>
#include <system_error>

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
    // Log the request
    std::cout << request.methodToString() << " " << request.path() << std::endl;

    // Check for matching route
    for(const auto& route : _routes) {
        if (route.method == request.method() && route.path == request.path()) {
            try {
                return route.handler(request);
            }
            catch(const std::exception& e) {
                std::cerr<<"Handler error: "<<e.what()<<std::endl;
                return HttpResponse::internalError("Internal Server Error");
            }
        }
    }

    // Check for static file serving
    for (const auto& [mountPath, directory] : _staticDirectories) {
        if (request.path().find(mountPath) == 0) {
            // Extract the file path
            std::string relativePath = request.path().substr(mountPath.length());
            if (relativePath.empty() || relativePath[0] != '/') {
                relativePath = "/" + relativePath;
            }

            std::string filePath = directory + relativePath;

            // Security check: prevent directory traversal
            std::filesystem::path fsPath(filePath);
            std::filesystem::path dirPath(directory);
            
            try {
                auto canonical = std::filesystem::canonical(fsPath);
                auto canonicalDir = std::filesystem::canonical(dirPath);
                
                // Check if the file is within the allowed directory
                auto mismatch = std::mismatch(
                    canonicalDir.begin(), canonicalDir.end(),
                    canonical.begin(), canonical.end()
                );

                if (mismatch.first != canonicalDir.end()) {
                    return HttpResponse::forbidden("Access Denied");
                }

                // Try to serve the file
                if (std::filesystem::exists(canonical) && 
                    std::filesystem::is_regular_file(canonical)) {
                    
                    std::ifstream file(canonical, std::ios::binary);

                    if (file) {
                        std::string content(
                            (std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>()
                        );
                        
                        auto mimeType = utils::getMimeType(filePath);
                        return HttpResponse::ok(content).contentType(mimeType);
                    }
                }
            } catch (const std::filesystem::filesystem_error&) {
                // File doesn't exist or can't be accessed
            }
        }
    }

    // No route found
    return HttpResponse::notFound("404 Not Found");
}

// ============================================================================
// HttpConnection Implementation
// ============================================================================

HttpConnection::HttpConnection(TcpSocket socket, HttpServer& server)
    : _socket(std::move(socket)), _server(server) {
}

void HttpConnection::start() {
    read();
}

void HttpConnection::read() {
    auto self(shared_from_this());

    boost::asio::async_read_until(
        _socket,
        _buffer,
        "\r\n\r\n",
        [this, self](std::error_code ec, std::size_t bytes) {
            if (!ec) {
                // Convert buffer to string
                std::istream stream(&_buffer);
                std::string header;
                std::string line;
                
                while (std::getline(stream, line) && line != "\r") {
                    header += line + "\n";
                }

                // Parse the header
                _request.parse(header);

                // Check if we need to read the body
                if (_request.contentLength() > 0) {
                    std::size_t remaining = _request.contentLength() - _buffer.size();
                    
                    if (remaining > 0) {
                        // Read the remaining body
                        boost::asio::async_read(
                            _socket,
                            _buffer,
                            boost::asio::transfer_exactly(remaining),
                            [this, self, &header](std::error_code ec, std::size_t) {
                                if (!ec) {
                                    // Now we have the complete request
                                    std::istream bodyStream(&_buffer);
                                    std::string body(
                                        (std::istreambuf_iterator<char>(bodyStream)),
                                        std::istreambuf_iterator<char>()
                                    );
                                    
                                    // Rebuild request with body
                                    std::string completeRequest = header + "\r\n" + body;
                                    _request.parse(completeRequest);
                                    
                                    processRequest();
                                } else {
                                    std::cerr << "Body read error: " << ec.message() << std::endl;
                                }
                            }
                        );
                        return;
                    }
                }

                processRequest();
            } else {
                std::cerr << "Read error: " << ec.message() << std::endl;
            }
        }
    );
}

void HttpConnection::processRequest() {
    // Handle the request and get response
    HttpResponse response = _server.handleRequest(_request);
    
    // Write the response
    write(response);
}

void HttpConnection::write(const HttpResponse& response) {
    auto self(shared_from_this());
    auto responseStr = std::make_shared<std::string>(response.toString());

    boost::asio::async_write(
        _socket,
        boost::asio::buffer(*responseStr),
        [this, self, responseStr](std::error_code ec, std::size_t) {
            if (ec) {
                std::cerr << "Write error: " << ec.message() << std::endl;
            }

            // Close connection after writing (HTTP/1.1 without keep-alive)
            boost::system::error_code shutdownEc;
            _socket.shutdown(TcpSocket::shutdown_both, shutdownEc);
            _socket.close();
        }
    );
}