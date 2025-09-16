#pragma once

#include "boost/asio.hpp"

using byte = unsigned char;

using TcpSocket     = boost::asio::ip::tcp::socket;
using TcpAcceptor   = boost::asio::ip::tcp::acceptor;
using IOContext     = boost::asio::io_context;
using TcpEndpoint   = boost::asio::ip::tcp::endpoint;
using TcpResolver   = boost::asio::ip::tcp::resolver;