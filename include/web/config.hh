#pragma once

#include <cstdint>
#include <string>

namespace web::http {
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
}